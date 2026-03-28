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
class ESP32Resource;
class ESP32Logger;

class ESP32Resource : public ResourceIfc
{
public:
  ~ESP32Resource() override = default;

  static auto create(LoggerIfc &logger,
                     std::string resourceString)
      -> std::unique_ptr<ESP32Resource>;

  auto write(const std::string &command) -> bool override;
  auto read() -> ReadResult override;
  auto query(const std::string &command) -> ReadResult override;

private:
  explicit ESP32Resource(LoggerIfc &logger,
                         std::string resourceIP,
                         uint16_t resourcePort) noexcept;
  auto connect() -> void;
  auto ensureConnected() -> void;
  [[nodiscard]] auto getFormattedIpPortPair() const -> std::string;

  LoggerIfc &logger_;
  const std::tuple<const std::string, const uint16_t> mIP_PortPair;
  int mSock{-1};
  const struct sockaddr_in mDestAddr{};
  std::array<char, COMM_BUFFER_SIZE> rx_buffer{};

  bool mIsOpen{false};
};
