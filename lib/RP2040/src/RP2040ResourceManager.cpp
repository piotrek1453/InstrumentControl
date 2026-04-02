#include "../inc/RP2040ResourceManager.hpp"
#include "../inc/RP2040Resource.hpp"
#include <memory>
#include <string>

RP2040ResourceManager::RP2040ResourceManager(
    LoggerIfc &logger)
    : logger_(logger)
{
}

auto RP2040ResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  return RP2040Resource::create(logger_, resourceString);
}
