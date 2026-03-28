#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string>

class RP2040ResourceManager : public ResourceManagerIfc
{
public:
  RP2040ResourceManager(LoggerIfc &logger);
  ~RP2040ResourceManager() override = default;

  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &logger_;
};
