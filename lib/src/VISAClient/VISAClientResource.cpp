#include "impl/VISAClient/VISAClientResource.hpp"
#include "ifc/ResourceIfc.hpp"
#include <fmt/core.h>
#include <string>
#include <string_view>

VISAClientResource::VISAClientResource(
    LoggerIfc &logger,
    std::string_view resource_string)
    : logger_(logger)
{
  logger_.log(fmt::format("Created VISAClientResource with resource string {}",
                          resource_string));
}

auto VISAClientResource::write(
    std::string_view command) -> bool
{
  // TODO: Implement write via VISA; placeholder logs
  logger_.log("VISAClientResource write: " + std::string(command));
  return true;
}

auto VISAClientResource::read() -> ReadResult
{
  // TODO: Implement read; placeholder returns empty success
  return ReadResult::success("");
}

auto VISAClientResource::query(
    std::string_view command) -> ReadResult
{
  // Simple placeholder: write then read
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
