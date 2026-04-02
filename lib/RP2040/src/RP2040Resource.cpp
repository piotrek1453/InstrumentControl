#include "../inc/RP2040Resource.hpp"
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <pico/time.h>
#include <socket.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <tuple>

RP2040Resource::~RP2040Resource() { closeConnection(); }

auto RP2040Resource::closeConnection() -> void
{
  close(static_cast<uint8_t>(getPort()));
  mIsOpen = false;
}

// assuming resourceString is in IP:port format
// i.e. "192.168.1.1:80"
auto RP2040Resource::create(
    LoggerIfc &logger,
    std::string resourceString) -> std::unique_ptr<RP2040Resource>
{
  size_t colonPos = resourceString.find(':');
  if (colonPos == std::string::npos)
  {
    logger.log("Incorrect resourceString: no colon found", LogLevel::Error);
    return nullptr;
  }

  std::array<uint8_t, 4> parsedIpOctets{};
  size_t octetIndex = 0;
  uint16_t octetValue = 0;
  size_t octetDigits = 0;
  constexpr uint16_t kDecimalBase = 10U;

  for (size_t i = 0; i <= colonPos; ++i)
  {
    const char currentChar = (i == colonPos) ? '.' : resourceString[i];
    if (currentChar >= '0' && currentChar <= '9')
    {
      octetValue =
          static_cast<uint16_t>((octetValue * kDecimalBase) +
                                static_cast<uint16_t>(currentChar - '0'));
      ++octetDigits;
      if (octetDigits > 3U || octetValue > std::numeric_limits<uint8_t>::max())
      {
        logger.log("Incorrect IP in resourceString", LogLevel::Error);
        return nullptr;
      }
      continue;
    }

    if (currentChar != '.' || octetDigits == 0U ||
        octetIndex >= parsedIpOctets.size())
    {
      logger.log("Incorrect IP in resourceString", LogLevel::Error);
      return nullptr;
    }

    parsedIpOctets[octetIndex] = static_cast<uint8_t>(octetValue);
    ++octetIndex;
    octetValue = 0;
    octetDigits = 0;
  }

  if (octetIndex != parsedIpOctets.size())
  {
    logger.log("Incorrect IP in resourceString", LogLevel::Error);
    return nullptr;
  }

  const char *portStr = resourceString.c_str() + colonPos + 1;
  if (*portStr == '\0')
  {
    logger.log("Incorrect port in resourceString", LogLevel::Error);
    return nullptr;
  }

  errno = 0;
  char *end = nullptr;
  const unsigned long parsedPort = std::strtoul(portStr, &end, 10);
  if (errno != 0 || *end != '\0' ||
      parsedPort > std::numeric_limits<uint16_t>::max())
  {
    logger.log("Incorrect port in resourceString", LogLevel::Error);
    return nullptr;
  }

  return std::unique_ptr<RP2040Resource>(new RP2040Resource(
      logger, parsedIpOctets, static_cast<uint16_t>(parsedPort)));
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

  if (socket(getPort(), Sn_MR_TCP, 0, 0) < 0)
  {
    logger_.log("Error creating socket, errno " + std::to_string(errno),
                LogLevel::Error);
    mIsOpen = false;
    return;
  }
  logger_.log("Socket created");

  // connecting to server
  logger_.log("Connecting to server on " + getFormattedIpPortPair());
  if (connect(getPort(), getIP().data(), getPort()) != 0)
  {
    logger_.log("Error connecting to host, errno " + std::to_string(errno),
                LogLevel::Error);
    closeConnection();
    return;
  }
  mIsOpen = true;
  logger_.log("Connected to server");
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
      status = getSn_SR(getPort());
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
  if (send(getPort(), txData, command.size()) < 0)
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

  mAvailable = getSn_RX_RSR(getPort());
  if (mAvailable > 0)
  {
    mAvailable = static_cast<int32_t>(std::min<uint32_t>(
        static_cast<uint32_t>(mAvailable), COMM_BUFFER_SIZE - 1));
    mRecv_len =
        recv(getPort(), rx_buffer.data(), static_cast<uint16_t>(mAvailable));
    if (mRecv_len > 0)
    {
      rx_buffer[static_cast<size_t>(mRecv_len)] = 0;
      readResult = ReadResult::success(
          std::string(reinterpret_cast<char *>(rx_buffer.data())));
      logger_.log("Received response: " + readResult.value);
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
