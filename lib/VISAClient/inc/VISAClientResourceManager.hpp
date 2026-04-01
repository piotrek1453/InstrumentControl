#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string>

class VISAClientResourceManager : public ResourceManagerIfc
{
public:
  VISAClientResourceManager(LoggerIfc &logger);
  ~VISAClientResourceManager() override = default;

  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string>;
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &logger_;
};
