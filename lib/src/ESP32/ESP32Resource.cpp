#include "impl/ESP32/ESP32Resource.hpp"

ESP32Resource::ESP32Resource(
    LoggerIfc &logger,
    const std::string &resource_string)
    : logger_(logger)
{
  // Avoid std::string concatenation that requires heap on MCU
  logger_.log("Created ESP32Resource");
  (void)resource_string; // TODO: store resource config
}

auto ESP32Resource::write(
    const std::string &command) -> bool
{
  // TODO: Write via UART/I2C/SPI
  logger_.log("ESP32Resource write");
  (void)command;
  return true;
}

auto ESP32Resource::read() -> ReadResult
{
  // TODO: Read via UART/I2C/SPI
  return ReadResult::success("");
}

auto ESP32Resource::query(
    const std::string &command) -> ReadResult
{
  if (!write(command))
  {
    return ReadResult::failure();
  }
  return read();
}
