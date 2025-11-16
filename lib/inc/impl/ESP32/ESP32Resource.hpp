#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string_view>

class ESP32Resource : public ResourceIfc {
public:
  explicit ESP32Resource(LoggerIfc &logger, std::string_view resource_string);
  ~ESP32Resource() override = default;

  auto write(std::string_view command) -> bool override;
  auto read() -> ReadResult override;
  auto query(std::string_view command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
