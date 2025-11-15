#include "impl/VISA/VISAResource.hpp"
#include <memory>
#include <string>

VISAResource::VISAResource(LoggerIfc &logger) : logger_(logger) {}

bool VISAResource::write(std::string_view command)
{
  // TODO: Implement write via VISA; placeholder logs
  logger_.log("VISAResource write: " + std::string(command));
  return true;
}

ReadResult VISAResource::read()
{
  // TODO: Implement read; placeholder returns empty success
  return ReadResult::success("");
}

ReadResult VISAResource::query(std::string_view command)
{
  // Simple placeholder: write then read
  if (!write(command))
    return ReadResult::failure();
  return read();
}
