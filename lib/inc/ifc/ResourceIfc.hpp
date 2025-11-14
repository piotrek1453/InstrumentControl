#pragma once

#include <optional>
#include <string>
#include <string_view>

class ResourceIfc {
public:
  virtual ~ResourceIfc() = default;

  virtual bool write(std::string_view command) = 0;
  virtual std::optional<std::string> read() = 0;
  virtual std::optional<std::string> query(std::string_view command) = 0;
};
