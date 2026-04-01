#include "../inc/ESP32Resource.hpp"
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <memory>
#include <stdint.h>
#include <string>
#include <sys/types.h>

namespace
{
// helper function so that initialization of
// sockaddr_in can be done at construction
auto makeDestAddr(
    const std::string &resourceIp,
    uint16_t resourcePort) -> sockaddr_in
{
  sockaddr_in destAddr{};
  destAddr.sin_addr.s_addr = inet_addr(resourceIp.c_str());
  destAddr.sin_family = AF_INET;
  destAddr.sin_port = htons(resourcePort);
  return destAddr;
}
} // namespace

ESP32Resource::~ESP32Resource() { closeConnection(); }

auto ESP32Resource::closeConnection() -> void
{
  if (mSock >= 0)
  {
    shutdown(mSock, SHUT_RDWR);
    close(mSock);
    mSock = -1;
  }
  mIsOpen = false;
}

// assuming resourceString is in IP:port format
// i.e. "192.168.1.1:80"
auto ESP32Resource::create(
    LoggerIfc &logger,
    std::string resourceString) -> std::unique_ptr<ESP32Resource>
{
  // split IP:port pair into separate variables
  // needed to keep compatibility with the interface
  std::string resourceIP;
  uint16_t resourcePort;

  size_t colonPos = resourceString.find(':');
  if (colonPos == std::string::npos)
  {
    logger.log("Incorrect resourceString: no colon found", LogLevel::Error);
    return nullptr;
  }
  resourceIP = resourceString.substr(0, colonPos);

  std::string portStr = resourceString.substr(colonPos + 1);
  if (portStr.empty())
  {
    logger.log("Incorrect port in resourceString", LogLevel::Error);
    return nullptr;
  }

  errno = 0;
  char *end = nullptr;
  const unsigned long parsedPort = std::strtoul(portStr.c_str(), &end, 10);
  if (errno != 0 || *end != '\0' ||
      parsedPort > std::numeric_limits<uint16_t>::max())
  {
    logger.log("Incorrect port in resourceString", LogLevel::Error);
    return nullptr;
  }
  resourcePort = static_cast<uint16_t>(parsedPort);

  return std::unique_ptr<ESP32Resource>(new ESP32Resource(
      logger, std::move(resourceIP), std::move(resourcePort)));
}

ESP32Resource::ESP32Resource(
    LoggerIfc &logger,
    std::string resourceIP,
    uint16_t resourcePort) noexcept
    : logger_(logger),
      mIP_PortPair(std::make_tuple(std::move(resourceIP),
                                   std::move(resourcePort))),
      mDestAddr(makeDestAddr(std::get<0>(mIP_PortPair),
                             std::get<1>(mIP_PortPair)))
{
  logger_.log("Created ESP32Resource with IP:port " +
              getFormattedIpPortPair());
}

auto ESP32Resource::connect() -> void
{
  if (mIsOpen)
  {
    return;
  }

  logger_.log("Setting up communication with server on " +
              getFormattedIpPortPair());

  // socket creation
  mSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (mSock < 0)
  {
    logger_.log("Error creating socket, errno " + std::to_string(errno),
                LogLevel::Error);
    mIsOpen = false;
    return;
  }
  logger_.log("Socket created");

  // connecting to server
  logger_.log("Connecting to server on " + getFormattedIpPortPair());
  if (lwip_connect(mSock, (struct sockaddr *)&mDestAddr, sizeof(mDestAddr)) !=
      0)
  {
    logger_.log("Error connecting to host, errno " + std::to_string(errno),
                LogLevel::Error);
    closeConnection();
    return;
  }
  mIsOpen = true;
  logger_.log("Connected to server");
}

auto ESP32Resource::ensureConnected() -> void
{
  if (!mIsOpen)
  {
    for (uint8_t attempt = 0; attempt < MAX_CONNECT_RETRIES && !mIsOpen;
         ++attempt)
    {
      logger_.log("Not connected to server, attempt " +
                      std::to_string(attempt + 1),
                  LogLevel::Warn);
      connect();
    }

    if (!mIsOpen)
    {
      logger_.log("Unable to connect to server after " +
                      std::to_string(MAX_CONNECT_RETRIES) + " attempts",
                  LogLevel::Error);
      return;
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

auto ESP32Resource::getFormattedIpPortPair() const -> std::string
{
  return std::get<0>(mIP_PortPair) + ':' +
         std::to_string(std::get<1>(mIP_PortPair));
}

auto ESP32Resource::write(
    const std::string &command) -> bool
{
  logger_.log("ESP32Resource write");

  ensureConnected();
  if (!mIsOpen)
  {
    logger_.log("Socket is not connected", LogLevel::Error);
    return false;
  }

  if (lwip_send(mSock, command.c_str(), command.size(), 0) < 0)
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

auto ESP32Resource::read() -> ReadResult
{
  ensureConnected();
  if (!mIsOpen)
  {
    return ReadResult::failure();
  }

  received = lwip_recv(mSock, rx_buffer.data(), sizeof(rx_buffer) - 1, 0);
  if (received > 0)
  {
    rx_buffer[received] = '\0';
    logger_.log("Received response: " + std::string(rx_buffer.data()));
    readResult = ReadResult::success(std::string(rx_buffer.data()));
  }
  else if (received == 0)
  {
    logger_.log("Connection closed by server");
    closeConnection();
    readResult = ReadResult::failure();
  }
  else
  {
    logger_.log("Response reception error, errno " + std::to_string(errno),
                LogLevel::Error);
    closeConnection();
    readResult = ReadResult::failure();
  }

  return readResult;
}

auto ESP32Resource::query(
    const std::string &command) -> ReadResult
{
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
