#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include "visa.h"
#include <memory>
#include <string_view>

class VISAResourceManager : public ResourceManagerIfc {
public:
  VISAResourceManager(std::shared_ptr<LoggerIfc> logger);
  ~VISAResourceManager() = default;

  std::vector<std::string> listAvailableResources() const override;
  std::shared_ptr<ResourceIfc>
  openResource(std::string_view resourceString) override;

private:
  std::shared_ptr<LoggerIfc> logger_;
};
