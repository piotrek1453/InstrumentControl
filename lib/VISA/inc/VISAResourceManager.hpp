#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string>
#include <visa.h>

class VISAResourceManager : public ResourceManagerIfc
{
public:
  /**
   * Opens a VISA resource manager session.
   */
  VISAResourceManager(LoggerIfc &logger) noexcept;

  /**
   * Closes the VISA resource manager session.
   */
  ~VISAResourceManager() override;

  /**
   * Enumerates VISA resource descriptors available in the system.
   */
  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;

  /**
   * Opens a resource for a VISA descriptor string.
   */
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  ViSession mResourceManagerHandle;
  LoggerIfc &mLogger;
};
