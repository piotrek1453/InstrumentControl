#pragma once
#include "ifc/LoggerIfc.hpp"
#include <memory>

#include "ifc/ResourceIfc.hpp"
class VISAResource : public ResourceIfc {
public:
  VISAResource(std::shared_ptr<LoggerIfc> logger);
  ~VISAResource() = default;

  bool write(std::string_view command) override;
  std::optional<std::string> read() override;
  std::optional<std::string> query(std::string_view command) override;

private:
  std::shared_ptr<LoggerIfc> logger_;
};
