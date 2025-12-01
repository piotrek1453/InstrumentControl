#include "impl/VISA/VISAResource.hpp"
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <array>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <visa.h>
#include <visatype.h>

auto VISAResource::create(
    LoggerIfc &logger,
    std::string resourceString,
    ViSession resourceManagerHandle) -> std::unique_ptr<VISAResource>
{
  auto resourcePointer = std::unique_ptr<VISAResource>(
      new VISAResource(logger, resourceString, resourceManagerHandle));

  if (!resourcePointer->mIsOpen)
  {
    return nullptr;
  }

  return resourcePointer;
}

VISAResource::VISAResource(
    LoggerIfc &logger,
    std::string resourceString,
    ViSession resourceManagerHandle) noexcept
    : logger_(logger),
      mResourceString(std::move(resourceString)),
      mResourceManagerHandle(resourceManagerHandle)
{
  auto status = viOpen(mResourceManagerHandle,
                       const_cast<ViRsrc>(mResourceString.c_str()),
                       INSTRUMENT_ACCESS_MODE,
                       INSTRUMENT_TIMEOUT_MS,
                       &mInstrumentSessionHandle);

  if (status != VI_SUCCESS)
  {
    std::array<ViChar, INSTRUMENT_BUFFER_SIZE_B> statusBuffer;
    auto statusDescription =
        viStatusDesc(mResourceManagerHandle, status, statusBuffer.data());
    logger.log(fmt::format("Error connecting to instrument\nResource string: "
                           "{}\nError: {} - {}",
                           mResourceString,
                           statusDescription,
                           statusBuffer.data()),
               LogLevel::Error);
    mIsOpen = false;
  }
  else
  {
    logger_.log(fmt::format("Created VISAResource with resource string {}",
                            mResourceString),
                LogLevel::Trace);
    mIsOpen = true;
  }
}

auto VISAResource::write(
    const std::string &command) -> bool
{
  auto status =
      viWrite(mInstrumentSessionHandle,
              reinterpret_cast<ViBuf>(const_cast<char *>(command.data())),
              static_cast<ViUInt32>(command.length()),
              &mIOBytes);
  if (status != VI_SUCCESS)
  {
    std::array<ViChar, INSTRUMENT_BUFFER_SIZE_B> statusBuffer;
    auto statusDescription =
        viStatusDesc(mResourceManagerHandle, status, statusBuffer.data());
    logger_.log(
        fmt::format(
            "Error writing to instrument\nResource string: {}\nError: {} - {}",
            mResourceString,
            statusDescription,
            statusBuffer.data()),
        LogLevel::Error);
    return false;
  }
  logger_.log(
      fmt::format(
          "Write to VISAResource succesful\nResource string: {}\nCommand: {}",
          mResourceString,
          command),
      LogLevel::Trace);
  return true;
}

auto VISAResource::read() -> ReadResult
{
  std::array<unsigned char, INSTRUMENT_BUFFER_SIZE_B> readBuffer;
  auto status = viRead(mInstrumentSessionHandle,
                       static_cast<ViBuf>(readBuffer.data()),
                       INSTRUMENT_BUFFER_SIZE_B,
                       &mIOBytes);
  if (status != VI_SUCCESS)
  {
    std::array<ViChar, INSTRUMENT_BUFFER_SIZE_B> statusBuffer;
    auto statusDescription =
        viStatusDesc(mResourceManagerHandle, status, statusBuffer.data());
    logger_.log(fmt::format("Error reading from instrument\nResource string: "
                            "{}\nError: {} - {}",
                            mResourceString,
                            statusDescription,
                            statusBuffer.data()),
                LogLevel::Error);
    return ReadResult::failure();
  }
  logger_.log(
      fmt::format(
          "Read from VISAResource succesful\nResource string: {}\nRead data: "
          "{}",
          mResourceString,
          std::string(reinterpret_cast<char *>(readBuffer.data()), mIOBytes)),
      LogLevel::Trace);
  return ReadResult::success(
      std::string(reinterpret_cast<char *>(readBuffer.data()), mIOBytes));
}

auto VISAResource::query(
    const std::string &command) -> ReadResult
{
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
