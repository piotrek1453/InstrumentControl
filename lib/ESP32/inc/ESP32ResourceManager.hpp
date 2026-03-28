#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceManagerIfc.hpp"
#include <memory>
#include <string>

class ESP32ResourceManager : public ResourceManagerIfc
{
public:
  /**
   * Creates a manager for ESP32 network resources.
   */
  ESP32ResourceManager(LoggerIfc &logger);
  ~ESP32ResourceManager() override = default;

  /**
   * Returns resources discoverable on this backend.
   *
   * The ESP32 backend does not implement runtime discovery, so the result is
   * an empty list.
   */
  [[nodiscard]] auto listAvailableResources() const
      -> std::vector<std::string> override;

  /**
   * Opens a resource addressed as "ip:port".
   */
  auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> override;

private:
  LoggerIfc &logger_;
};
