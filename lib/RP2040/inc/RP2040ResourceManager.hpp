#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string>

struct RP2040NetworkConfig
{
  std::array<uint8_t, 6> mac{};
  std::array<uint8_t, 4> ip{};
  std::array<uint8_t, 4> subnetMask{};
  std::array<uint8_t, 4> gateway{};
  std::array<uint8_t, 4> dns{};
  bool useDhcp{false};
};

class RP2040ResourceManager : public ResourceManagerIfc
{
public:
  static auto create(LoggerIfc &logger,
                     const RP2040NetworkConfig &networkConfig)
      -> std::unique_ptr<RP2040ResourceManager>;

  ~RP2040ResourceManager() override = default;

  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  explicit RP2040ResourceManager(LoggerIfc &logger);

  LoggerIfc &mLogger;
  bool isNetworkInitialized_{false};
};
