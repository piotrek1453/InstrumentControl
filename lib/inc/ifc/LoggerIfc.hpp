#pragma once
#include <string_view>

enum class LogLevel
{
  Info,
  Warn,
  Error,
  Debug
};

class LoggerIfc
{
public:
  virtual ~LoggerIfc() = default;

  virtual void log(std::string_view message, LogLevel level = LogLevel::Info) = 0;
};
