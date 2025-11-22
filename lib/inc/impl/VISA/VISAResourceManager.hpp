#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string_view>
#include <visa.h>

class VISAResourceManager : public ResourceManagerIfc {
public:
  VISAResourceManager(LoggerIfc &logger) noexcept;
  ~VISAResourceManager() override;

  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;
  auto openResource(std::string_view resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  ViSession mResourceManager;
  LoggerIfc &mLogger;
};
