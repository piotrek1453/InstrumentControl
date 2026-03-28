#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string>

class RP2040Resource : public ResourceIfc
{
public:
  explicit RP2040Resource(LoggerIfc &logger,
                          const std::string &resource_string);
  ~RP2040Resource() override = default;

  auto write(const std::string &command) -> bool override;
  auto read() -> ReadResult override;
  auto query(const std::string &command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
