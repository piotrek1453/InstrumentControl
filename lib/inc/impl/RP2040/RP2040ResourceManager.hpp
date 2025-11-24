#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string_view>

class RP2040ResourceManager : public ResourceManagerIfc
{
public:
  RP2040ResourceManager(LoggerIfc &logger);
  ~RP2040ResourceManager() override = default;

  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;
  auto openResource(std::string_view resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &logger_;
};
