#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string_view>

class ESP32ResourceManager : public ResourceManagerIfc {
public:
  ESP32ResourceManager(LoggerIfc &logger);
  ~ESP32ResourceManager() override = default;

  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;
  auto openResource(std::string_view resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &logger_;
};
