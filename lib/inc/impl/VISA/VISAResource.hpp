#pragma once
#include "ifc/LoggerIfc.hpp"
#include <memory>
#include "ifc/ResourceIfc.hpp"

class VISAResource : public ResourceIfc
{
public:
  explicit VISAResource(LoggerIfc &logger);
  ~VISAResource() = default;

  bool write(std::string_view command) override;
  ReadResult read() override;
  ReadResult query(std::string_view command) override;

private:
  LoggerIfc &logger_;
};
