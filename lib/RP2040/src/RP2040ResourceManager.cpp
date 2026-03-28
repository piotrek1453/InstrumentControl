#include "../inc/RP2040ResourceManager.hpp"
#include "../inc/RP2040Resource.hpp"
#include <memory>
#include <string>
#include <vector>

RP2040ResourceManager::RP2040ResourceManager(
    LoggerIfc &logger)
    : logger_(logger)
{
}

auto RP2040ResourceManager::listAvailableResources() const
    -> std::vector<std::string>
{
  return {};
}

auto RP2040ResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  return std::make_unique<RP2040Resource>(logger_, resourceString);
}
