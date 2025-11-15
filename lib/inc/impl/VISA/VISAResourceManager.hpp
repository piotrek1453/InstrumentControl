#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include "visa.h"
#include <memory>
#include <string_view>

class VISAResourceManager : public ResourceManagerIfc
{
public:
  VISAResourceManager(LoggerIfc &logger);
  ~VISAResourceManager() = default;

  std::vector<std::string> listAvailableResources() const override;
  std::unique_ptr<ResourceIfc>
  openResource(std::string_view resourceString) override;

private:
  LoggerIfc &logger_;
};
