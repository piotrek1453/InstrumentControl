#pragma once
#include <string_view>

class LoggerIfc {
public:
  virtual ~LoggerIfc() = default;

  virtual void info(std::string_view message) = 0;
  virtual void warn(std::string_view message) = 0;
  virtual void error(std::string_view message) = 0;
  virtual void debug(std::string_view message) = 0;
};
