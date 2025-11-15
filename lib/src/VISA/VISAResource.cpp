#include "impl/VISA/VISAResource.hpp"
#include "ifc/ResourceIfc.hpp"
#include <fmt/core.h>
#include <string>
#include <string_view>

VISAResource::VISAResource(LoggerIfc &logger, std::string_view resource_string)
    : logger_(logger) {
  logger_.log(fmt::format("Created VISAResource with resource string {}",
                          resource_string));
}

auto VISAResource::write(std::string_view command) -> bool {
  // TODO: Implement write via VISA; placeholder logs
  logger_.log("VISAResource write: " + std::string(command));
  return true;
}

auto VISAResource::read() -> ReadResult {
  // TODO: Implement read; placeholder returns empty success
  return ReadResult::success("");
}

auto VISAResource::query(std::string_view command) -> ReadResult {
  // Simple placeholder: write then read
  if (!write(command)) {
    return ReadResult::failure();
  }
  return read();
}
