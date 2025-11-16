#include "impl/ESP32/ESP32Resource.hpp"

ESP32Resource::ESP32Resource(LoggerIfc &logger,
                             std::string_view resource_string)
    : logger_(logger) {
  // Avoid std::string concatenation that requires heap on MCU
  logger_.log("Created ESP32Resource");
  (void)resource_string; // TODO: store resource config
}

auto ESP32Resource::write(std::string_view command) -> bool {
  // TODO: Write via UART/I2C/SPI
  logger_.log("ESP32Resource write");
  (void)command;
  return true;
}

auto ESP32Resource::read() -> ReadResult {
  // TODO: Read via UART/I2C/SPI
  return ReadResult::success("");
}

auto ESP32Resource::query(std::string_view command) -> ReadResult {
  if (!write(command)) {
    return ReadResult::failure();
  }
  return read();
}
