#include "impl/VISA/VISAResourceManager.hpp"
#include "impl/VISA/VISAResource.hpp"
#include <memory>
#include <string>
#include <vector>

VISAResourceManager::VISAResourceManager(LoggerIfc &logger) : logger_(logger) {}

auto VISAResourceManager::listAvailableResources() const
    -> std::vector<std::string> {
  // TODO: Implement VISA enumeration; placeholder empty
  return {};
}

auto VISAResourceManager::openResource(std::string_view resourceString)
    -> std::unique_ptr<ResourceIfc> {
  // TODO: Use VISA API to open; placeholder creates stub resource
  return std::make_unique<VISAResource>(logger_, resourceString);
}
