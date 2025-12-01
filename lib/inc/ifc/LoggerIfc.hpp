#pragma once
#include <cstdint>
#include <string>

enum class LogLevel : uint32_t
{
  Info,
  Warn,
  Error,
  Debug,
  Trace
};

class LoggerIfc
{
public:
  virtual ~LoggerIfc() = default;

  virtual void setLoggingLevel(LogLevel level) = 0;
  virtual void log(const std::string &message,
                   LogLevel level = LogLevel::Info) = 0;
};
