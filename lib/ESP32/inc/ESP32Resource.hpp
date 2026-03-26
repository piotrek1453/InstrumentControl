#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string>

class ESP32Resource : public ResourceIfc
{
public:
  explicit ESP32Resource(LoggerIfc &logger,
                         const std::string &resource_string);
  ~ESP32Resource() override = default;

  auto write(const std::string &command) -> bool override;
  auto read() -> ReadResult override;
  auto query(const std::string &command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
