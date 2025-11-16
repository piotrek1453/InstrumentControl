#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string_view>

class RP2040Resource : public ResourceIfc {
public:
  explicit RP2040Resource(LoggerIfc &logger, std::string_view resource_string);
  ~RP2040Resource() override = default;

  auto write(std::string_view command) -> bool override;
  auto read() -> ReadResult override;
  auto query(std::string_view command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
