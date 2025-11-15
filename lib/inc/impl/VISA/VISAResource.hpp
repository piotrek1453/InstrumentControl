#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string_view>

class VISAResource : public ResourceIfc {
public:
  explicit VISAResource(LoggerIfc &logger, std::string_view resource_string);
  ~VISAResource() override = default;

  auto write(std::string_view command) -> bool override;
  auto read() -> ReadResult override;
  auto query(std::string_view command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
