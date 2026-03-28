#pragma once
#include "ifc/LoggerIfc.hpp"
#include "ifc/ResourceIfc.hpp"
#include <string>

class VISAClientResource : public ResourceIfc
{
public:
  explicit VISAClientResource(LoggerIfc &logger,
                              const std::string &resource_string);
  ~VISAClientResource() override = default;

  auto write(const std::string &command) -> bool override;
  auto read() -> ReadResult override;
  auto query(const std::string &command) -> ReadResult override;

private:
  LoggerIfc &logger_;
};
