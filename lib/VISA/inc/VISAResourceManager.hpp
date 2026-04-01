#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string>
#include <visa.h>

class VISAResourceManager : public ResourceManagerIfc
{
public:
  VISAResourceManager(LoggerIfc &logger) noexcept;
  ~VISAResourceManager() override;

  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string>;
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  ViSession mResourceManagerHandle;
  LoggerIfc &mLogger;
};
