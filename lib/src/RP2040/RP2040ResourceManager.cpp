#include "impl/RP2040/RP2040ResourceManager.hpp"
#include "impl/RP2040/RP2040Resource.hpp"
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
  // TODO: Implement VISA enumeration; placeholder empty
  return {};
}

auto RP2040ResourceManager::openResource(
    std::string_view resourceString) -> std::unique_ptr<ResourceIfc>
{
  // TODO: Use VISA API to open; placeholder creates stub resource
  return std::make_unique<RP2040Resource>(logger_, resourceString);
}
