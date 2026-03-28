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
  return {};
}

auto ESP32ResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  return ESP32Resource::create(logger_, resourceString);
}
