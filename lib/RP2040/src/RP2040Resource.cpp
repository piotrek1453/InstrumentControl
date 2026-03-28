#include "../inc/RP2040Resource.hpp"

RP2040Resource::RP2040Resource(
    LoggerIfc &logger,
    const std::string &resource_string)
    : logger_(logger)
{
  logger_.log("Created RP2040Resource");
  (void)resource_string;
}

auto RP2040Resource::write(
    const std::string &command) -> bool
{
  logger_.log("RP2040Resource write");
  (void)command;
  return true;
}

auto RP2040Resource::read() -> ReadResult { return ReadResult::success(""); }

auto RP2040Resource::query(
    const std::string &command) -> ReadResult
{
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
