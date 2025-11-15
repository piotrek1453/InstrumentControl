#include "impl/VISA/VISAResourceManager.hpp"
#include "impl/VISA/VISAResource.hpp"
#include <memory>
#include <vector>
#include <string>

VISAResourceManager::VISAResourceManager(LoggerIfc &logger) : logger_(logger) {}

std::vector<std::string> VISAResourceManager::listAvailableResources() const
{
  // TODO: Implement VISA enumeration; placeholder empty
  return {};
}

std::unique_ptr<ResourceIfc>
VISAResourceManager::openResource(std::string_view resourceString)
{
  // TODO: Use VISA API to open; placeholder creates stub resource
  return std::make_unique<VISAResource>(logger_);
}
