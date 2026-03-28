#include "../inc/ESP32ResourceManager.hpp"
#include "../inc/ESP32Resource.hpp"
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

// assuming resourceString is in IP:port format
// i.e. "192.168.1.1:80"
auto ESP32ResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  return ESP32Resource::create(logger_, resourceString);
}
