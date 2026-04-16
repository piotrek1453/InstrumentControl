#include "../inc/RP2040Resource.hpp"
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceAddressParser.hpp"
#include "ifc/ResourceIfc.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <pico/time.h>
#include <socket.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <tuple>

namespace
{
constexpr uint8_t kSocketId{0};
constexpr uint8_t kConnectStatusPolls{25};
constexpr uint16_t kConnectStatusPollDelayMs{20};
} // namespace

RP2040Resource::~RP2040Resource() { closeConnection(); }

auto RP2040Resource::closeConnection() -> void
{
  close(kSocketId);
  mIsOpen = false;
}

// assuming resourceString is in IP:port format
// i.e. "192.168.1.1:80"
auto RP2040Resource::create(
    LoggerIfc &logger,
    std::string resourceString) -> std::unique_ptr<RP2040Resource>
{
  const auto parseResult = parseResourceAddress(resourceString);
  if (!parseResult.isOk)
  {
    if (parseResult.error == ResourceAddressParseError::MissingColon)
    {
      logger.log("Incorrect resourceString: no colon found", LogLevel::Error);
    }
    else if (parseResult.error == ResourceAddressParseError::EmptyPort ||
             parseResult.error == ResourceAddressParseError::InvalidPort)
    {
      logger.log("Incorrect port in resourceString", LogLevel::Error);
    }
    else
    {
      logger.log("Incorrect IP in resourceString", LogLevel::Error);
    }
    return nullptr;
  }

  return std::unique_ptr<RP2040Resource>(new RP2040Resource(
      logger, parseResult.value.ipOctets, parseResult.value.port));
}

RP2040Resource::RP2040Resource(
    LoggerIfc &logger,
    std::array<uint8_t,
               4> resourceIP,
    uint16_t resourcePort) noexcept
    : logger_(logger),
      mIP_PortPair(std::make_tuple(std::move(resourceIP),
                                   std::move(resourcePort)))
{
  logger_.log("Created RP2040Resource");
}

auto RP2040Resource::openSocketConnection() -> void
{
  if (mIsOpen)
  {
    return;
  }

  logger_.log("Setting up communication with server on " +
              getFormattedIpPortPair());

  // socket opening

  if (socket(kSocketId, Sn_MR_TCP, 0, 0) < 0)
  {
    logger_.log("Error creating socket, errno " + std::to_string(errno),
                LogLevel::Error);
    mIsOpen = false;
    return;
  }
  logger_.log("Socket created");

  // connecting to server
  logger_.log("Connecting to server on " + getFormattedIpPortPair());
  const int32_t connectResult = connect(kSocketId, getIP().data(), getPort());
  if (connectResult == SOCK_OK)
  {
    mIsOpen = true;
    logger_.log("Connected to server");
    return;
  }

  if (connectResult == SOCK_BUSY)
  {
    for (uint8_t poll = 0; poll < kConnectStatusPolls; ++poll)
    {
      const uint8_t status = getSn_SR(kSocketId);
      if (status == SOCK_ESTABLISHED)
      {
        mIsOpen = true;
        logger_.log("Connected to server");
        return;
      }

      if (status == SOCK_CLOSE_WAIT || status == SOCK_CLOSED)
      {
        logger_.log("Connect failed during SOCK_BUSY, status " +
                        std::to_string(status),
                    LogLevel::Error);
        closeConnection();
        return;
      }

      sleep_ms(kConnectStatusPollDelayMs);
    }

    logger_.log("Connect timeout waiting for SOCK_ESTABLISHED",
                LogLevel::Error);
    closeConnection();
    return;
  }

  if (connectResult != SOCK_OK)
  {
    logger_.log("Error connecting to host, ret " +
                    std::to_string(connectResult) + ", sn_sr " +
                    std::to_string(getSn_SR(kSocketId)) + ", errno " +
                    std::to_string(errno),
                LogLevel::Error);
    closeConnection();
    return;
  }
}

auto RP2040Resource::ensureConnected() -> void
{
  uint8_t status = SOCK_CLOSED;
  if (!mIsOpen)
  {
    for (uint8_t attempt = 0; attempt < MAX_CONNECT_RETRIES && !mIsOpen;
         ++attempt)
    {
      logger_.log("Not connected to server, attempt " +
                      std::to_string(attempt + 1),
                  LogLevel::Warn);

      openSocketConnection();
      status = getSn_SR(kSocketId);
      if (status == SOCK_CLOSE_WAIT || status == SOCK_CLOSED)
      {
        closeConnection();
        sleep_ms(200);
      }
    }

    if (!mIsOpen || status != SOCK_ESTABLISHED)
    {
      logger_.log("Unable to connect to server after " +
                      std::to_string(MAX_CONNECT_RETRIES) + " attempts",
                  LogLevel::Error);
      return;
    }
  }
}

auto RP2040Resource::getFormattedIpPortPair() const -> std::string
{
  const auto ipAddress = getIP();
  return std::to_string(ipAddress[0]) + '.' + std::to_string(ipAddress[1]) +
         '.' + std::to_string(ipAddress[2]) + '.' +
         std::to_string(ipAddress[3]) + ':' + std::to_string(getPort());
}

auto RP2040Resource::getIP() const -> std::array<uint8_t,
                                                 4>
{
  return std::get<0>(mIP_PortPair);
}

auto RP2040Resource::getPort() const -> uint16_t
{
  return std::get<1>(mIP_PortPair);
}

auto RP2040Resource::write(
    const std::string &command) -> bool
{
  ensureConnected();
  if (!mIsOpen)
  {
    logger_.log("Socket is not connected", LogLevel::Error);
    return false;
  }
  logger_.log("RP2040Resource write");

  // ioLibrary send expects uint8_t*, even though it does not modify payload.
  auto *txData =
      const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(command.data()));
  if (send(kSocketId, txData, command.size()) < 0)
  {
    logger_.log("Error sending message: \"" + command + "\", errno " +
                    std::to_string(errno),
                LogLevel::Error);
    closeConnection();
    return false;
  }
  logger_.log("Sent message: \"" + command + '\"');
  return true;
}

auto RP2040Resource::read() -> ReadResult
{
  ensureConnected();
  readResult = ReadResult::failure();
  if (!mIsOpen)
  {
    logger_.log("Socket is not connected", LogLevel::Error);
    return readResult;
  }
  logger_.log("RP2040Resource read");

  mAvailable = getSn_RX_RSR(kSocketId);
  if (mAvailable > 0)
  {
    mAvailable = static_cast<int32_t>(std::min<uint32_t>(
        static_cast<uint32_t>(mAvailable), COMM_BUFFER_SIZE - 1));
    mRecv_len =
        recv(kSocketId, rx_buffer.data(), static_cast<uint16_t>(mAvailable));
    if (mRecv_len > 0)
    {
      rx_buffer[static_cast<size_t>(mRecv_len)] = 0;
      readResult = ReadResult::success(
          std::string(reinterpret_cast<char *>(rx_buffer.data())));
      logger_.log("Received response: \"" + readResult.value + '\"');
    }
    else
    {
      logger_.log("No message received", LogLevel::Error);
      closeConnection();
    }
  }

  return readResult;
}

auto RP2040Resource::query(
    const std::string &command) -> ReadResult
{
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
