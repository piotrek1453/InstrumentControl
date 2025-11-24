#include "impl/VISA/VISAResource.hpp"
#include "ifc/ResourceIfc.hpp"
#include <fmt/core.h>
#include <string>
#include <string_view>
#include <visa.h>
#include <visatype.h>

VISAResource::VISAResource(
    LoggerIfc &logger,
    std::string_view resourceString,
    ViSession resourceManagerHandle) noexcept
    : logger_(logger),
      mResourceString(resourceString),
      mResourceManagerHandle(resourceManagerHandle)
{
  auto status = viOpen(mResourceManagerHandle,
                       const_cast<ViRsrc>(mResourceString.data()),
                       INSTRUMENT_ACCESS_MODE,
                       INSTRUMENT_TIMEOUT_MS,
                       &mInstrumentSessionHandle);

  if (status != VI_SUCCESS)
  {
    logger.log(fmt::format(
        "Error connecting to instrument with resource "
        "string {}\nError: {}",
        mResourceString,
        viStatusDesc(mResourceManagerHandle, status, mBuffer.data())));
    mIsOpen = false;
  }
  else
  {
    logger_.log(fmt::format("Created VISAResource with resource string {}",
                            resourceString));
    mIsOpen = true;
  }
}

auto VISAResource::write(
    std::string_view command) -> bool
{
  // TODO: Implement write via VISA; placeholder logs
  logger_.log("VISAResource write: " + std::string(command));
  return true;
}

auto VISAResource::read() -> ReadResult
{
  // TODO: Implement read; placeholder returns empty success
  return ReadResult::success("");
}

auto VISAResource::query(
    std::string_view command) -> ReadResult
{
  // Simple placeholder: write then read
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
