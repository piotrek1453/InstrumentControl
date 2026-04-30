#include "../inc/ESP32ResourceManager.hpp"
#include "../inc/ESP32Resource.hpp"
#include <memory>
#include <string>

ESP32ResourceManager::ESP32ResourceManager(
    LoggerIfc &logger)
    : mLogger(logger)
{
}

// assuming resourceString is in IP:port format
// i.e. "192.168.1.1:80"
auto ESP32ResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  return ESP32Resource::create(mLogger, resourceString);
}
