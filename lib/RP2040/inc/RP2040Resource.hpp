#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string>

class RP2040Resource : public ResourceIfc
{
public:
  /**
   * Creates an RP2040 resource bound to a textual descriptor.
   */
  explicit RP2040Resource(LoggerIfc &logger,
                          const std::string &resource_string);
  ~RP2040Resource() override = default;

  /**
   * Writes a command to the transport.
   *
   * The transport layer is currently a placeholder implementation.
   */
  auto write(const std::string &command) -> bool override;

  /**
   * Reads a response from the transport.
   *
   * The transport layer is currently a placeholder implementation.
   */
  auto read() -> ReadResult override;

  /**
   * Writes and then reads in one call.
   */
  auto query(const std::string &command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
