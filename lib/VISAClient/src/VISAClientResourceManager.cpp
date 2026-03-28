#include "../inc/VISAClientResourceManager.hpp"
#include "../inc/VISAClientResource.hpp"
#include <memory>
#include <string>
#include <vector>

VISAClientResourceManager::VISAClientResourceManager(
    LoggerIfc &logger)
    : logger_(logger)
{
}

auto VISAClientResourceManager::listAvailableResources() const
    -> std::vector<std::string>
{
  // TODO: Implement VISA enumeration; placeholder empty
  return {};
}

auto VISAClientResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  // TODO: Use VISA API to open; placeholder creates stub resource
  return std::make_unique<VISAClientResource>(logger_, resourceString);
}
