#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string>

class VISAClientResource : public ResourceIfc
{
public:
  /**
   * Creates a VISA-client resource bound to a descriptor.
   */
  explicit VISAClientResource(LoggerIfc &logger,
                              const std::string &resource_string);
  ~VISAClientResource() override = default;

  /**
   * Sends a command to the transport.
   *
   * Current implementation is a placeholder that logs command text.
   */
  auto write(const std::string &command) -> bool override;

  /**
   * Reads a response from the transport.
   *
   * Current implementation is a placeholder that returns an empty success.
   */
  auto read() -> ReadResult override;

  /**
   * Executes write followed by read.
   */
  auto query(const std::string &command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
