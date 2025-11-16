#include "impl/RP2040/RP2040Resource.hpp"

RP2040Resource::RP2040Resource(LoggerIfc &logger,
                               std::string_view resource_string)
    : logger_(logger) {
  // Avoid std::string concatenation that requires heap on MCU
  logger_.log("Created RP2040Resource");
  (void)resource_string; // TODO: store resource config
}

auto RP2040Resource::write(std::string_view command) -> bool {
  // TODO: Write via UART/I2C/SPI
  logger_.log("RP2040Resource write");
  (void)command;
  return true;
}

auto RP2040Resource::read() -> ReadResult {
  // TODO: Read via UART/I2C/SPI
  return ReadResult::success("");
}

auto RP2040Resource::query(std::string_view command) -> ReadResult {
  if (!write(command)) {
    return ReadResult::failure();
  }
  return read();
}
