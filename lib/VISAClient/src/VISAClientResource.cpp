#include "../inc/VISAClientResource.hpp"
#include "ifc/ResourceIfc.hpp"
#include <fmt/core.h>
#include <string>

VISAClientResource::VISAClientResource(
    LoggerIfc &logger,
    const std::string &resource_string)
    : logger_(logger)
{
  logger_.log(fmt::format("Created VISAClientResource with resource string {}",
                          resource_string));
}

auto VISAClientResource::write(
    const std::string &command) -> bool
{
  // TODO: Implement write via VISA; placeholder logs
  logger_.log("VISAClientResource write: " + command);
  return true;
}

auto VISAClientResource::read() -> ReadResult
{
  // TODO: Implement read; placeholder returns empty success
  return ReadResult::success("");
}

auto VISAClientResource::query(
    const std::string &command) -> ReadResult
{
  // Simple placeholder: write then read
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
