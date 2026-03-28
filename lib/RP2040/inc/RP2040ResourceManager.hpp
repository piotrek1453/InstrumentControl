#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string>

class RP2040ResourceManager : public ResourceManagerIfc
{
public:
  /**
   * Creates a manager for RP2040 resources.
   */
  RP2040ResourceManager(LoggerIfc &logger);
  ~RP2040ResourceManager() override = default;

  /**
   * Returns resources discoverable on this backend.
   *
   * Discovery is not implemented, so the result is empty.
   */
  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;

  /**
   * Creates a resource for the provided descriptor.
   */
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &logger_;
};
