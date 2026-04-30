#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

constexpr uint32_t COMM_BUFFER_SIZE{256};
constexpr uint8_t MAX_CONNECT_RETRIES{5};

class RP2040Resource : public ResourceIfc
{
public:
  /**
   * Closes the underlying socket if it is open.
   */
  ~RP2040Resource();

  /**
   * Creates a TCP-backed resource from an "ip:port" descriptor.
   *
   * Returns nullptr when parsing fails.
   */
  static auto create(LoggerIfc &logger,
                     std::string resourceString)
      -> std::unique_ptr<RP2040Resource>;

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
  explicit RP2040Resource(LoggerIfc &logger,
                          std::array<uint8_t,
                                     4> resourceIP,
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
  [[nodiscard]] auto getIP() const -> std::array<uint8_t,
                                                 4>;

  /**
   * Returns server port.
   */
  [[nodiscard]] auto getPort() const -> uint16_t;

  LoggerIfc &mLogger;
  // not const because connect() takes
  // non-const uint8_t* for IP
  std::tuple<std::array<uint8_t, 4>, const uint16_t> mIP_PortPair;
  std::array<uint8_t, COMM_BUFFER_SIZE> rx_buffer{};
  int32_t mAvailable{0};
  int32_t mRecv_len{0};

  bool mIsOpen{false};
  ReadResult readResult;
};
