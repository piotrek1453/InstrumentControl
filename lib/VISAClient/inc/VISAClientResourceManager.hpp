#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string>

class VISAClientResourceManager : public ResourceManagerIfc
{
public:
  /**
   * Creates a VISA-client resource manager.
   */
  VISAClientResourceManager(LoggerIfc &logger);
  ~VISAClientResourceManager() override = default;

  /**
   * Returns discoverable resources.
   *
   * Discovery is currently a placeholder and returns an empty list.
   */
  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;

  /**
   * Opens a resource for a provided descriptor.
   *
   * Current implementation returns a placeholder resource.
   */
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &logger_;
};
