#include "../inc/VISAResourceManager.hpp"
#include "../inc/VISAResource.hpp"
#include "ifc/LoggerIfc.hpp"
#include <array>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <vector>
#include <visa.h>
#include <visatype.h>

VISAResourceManager::VISAResourceManager(
    LoggerIfc &logger) noexcept
    : mLogger(logger)
{
  auto status = viOpenDefaultRM(&mResourceManagerHandle);
  mLogger.log(fmt::format(
      "Opening VISA resource manager finished with status {}", status));
}

VISAResourceManager::~VISAResourceManager()
{
  viClear(mResourceManagerHandle);
  viClose(mResourceManagerHandle);
}

auto VISAResourceManager::listAvailableResources() const
    -> std::vector<std::string>
{
  ViFindList findList{};
  ViUInt32 numInstrs{};
  std::array<char, VI_FIND_BUFLEN> instrDescriptor{};
  std::vector<std::string> availableResources{};

  auto status = viFindRsrc(mResourceManagerHandle,
                           const_cast<ViChar *>("?*"),
                           &findList,
                           &numInstrs,
                           instrDescriptor.data());
  if (status != VI_SUCCESS)
  {
    mLogger.log(
        fmt::format("Error discovering resources, status: {}", status));
    return {};
  }

  do
  {
    availableResources.emplace_back(instrDescriptor.data());
    mLogger.log(
        fmt::format("Enumerating VISA resource finished with status: "
                    "{}\nObtained instrument descriptor: {}\nNumber of "
                    "instruments left to enumerate: {}",
                    status,
                    instrDescriptor.data(),
                    numInstrs - 1),
        LogLevel::Trace);

    instrDescriptor = {};
    status = viFindNext(findList, instrDescriptor.data());
  } while (--numInstrs > 0);

  return availableResources;
}

auto VISAResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  return VISAResource::create(mLogger, resourceString, mResourceManagerHandle);
}
