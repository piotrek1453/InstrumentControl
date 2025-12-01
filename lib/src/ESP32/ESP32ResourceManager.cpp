#include "impl/ESP32/ESP32ResourceManager.hpp"
#include "impl/ESP32/ESP32Resource.hpp"
#include <memory>
#include <string>
#include <vector>

ESP32ResourceManager::ESP32ResourceManager(
    LoggerIfc &logger)
    : logger_(logger)
{
}

auto ESP32ResourceManager::listAvailableResources() const
    -> std::vector<std::string>
{
  // TODO: Implement VISA enumeration; placeholder empty
  return {};
}

auto ESP32ResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  // TODO: Use VISA API to open; placeholder creates stub resource
  return std::make_unique<ESP32Resource>(logger_, resourceString);
}
