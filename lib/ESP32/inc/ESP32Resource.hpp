#pragma once
#include "ESP32/inc/ESP32Logger.hpp"
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include <array>
#include <memory>
#include <stdint.h>
#include <string>
#include <tuple>

constexpr uint32_t COMM_BUFFER_SIZE{256};
constexpr uint8_t MAX_CONNECT_RETRIES{5};

class ESP32Resource;
class ESP32Logger;

class ESP32Resource : public ResourceIfc
{
public:
  /**
   * Closes the underlying socket if it is open.
   */
  ~ESP32Resource();

  /**
   * Creates a TCP-backed resource from an "ip:port" descriptor.
   *
   * Returns nullptr when parsing fails.
   */
  static auto create(LoggerIfc &logger,
                     std::string resourceString)
      -> std::unique_ptr<ESP32Resource>;

  /**
   * Sends command data through the configured transport.*/
  auto write(const std::string &command) -> bool override;

  /**
   * Reads response data from the configured transport.*/
  auto read() -> ReadResult override;

  /**
   * Executes write followed by read.*/
  auto query(const std::string &command) -> ReadResult override;

private:
  explicit ESP32Resource(LoggerIfc &logger,
                         std::string resourceIP,
                         uint16_t resourcePort) noexcept;

  /**
   * Establishes a TCP connection to the configured endpoint.
   */
  auto openSocketConnection() -> void;

  /**
   * Terminates a TCP connection to the configured endpoint.
   */
  auto closeConnection() -> void;

  /**
   * Retries connection until success or retry budget is exhausted.
   */
  auto ensureConnected() -> void;

  /**
   * Returns endpoint text in "ip:port" form.
   */
  [[nodiscard]] auto getFormattedIpPortPair() const -> std::string;

  /**
   * Returns server IP.
   */
  [[nodiscard]] auto getIP() const -> std::string;

  /**
   * Returns server port.
   */
  [[nodiscard]] auto getPort() const -> uint16_t;

  LoggerIfc &logger_;
  const std::tuple<const std::string, const uint16_t> mIP_PortPair;
  int mSock{-1};
  const struct sockaddr_in mDestAddr{};
  std::array<char, COMM_BUFFER_SIZE> rx_buffer{};

  bool mIsOpen{false};
  ReadResult readResult;
  int received;
};
