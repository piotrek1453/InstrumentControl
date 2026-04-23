#pragma once
#include <cstdint>
#include <source_location>
#include <string>

enum class LogLevel : uint32_t
{
  Error,
  Warn,
  Info,
  Debug,
  Trace
};

constexpr auto logLevelToString(
    LogLevel level) -> const char *
{
  switch (level)
  {
  case LogLevel::Trace:
    return "TRACE";
  case LogLevel::Debug:
    return "DEBUG";
  case LogLevel::Error:
    return "ERROR";
  case LogLevel::Warn:
    return "WARN";
  case LogLevel::Info:
    return "INFO";
  default:
    return "UNKNOWN LOG LEVEL";
  }
}

class LoggerIfc
{
public:
  virtual ~LoggerIfc() = default;

  virtual void setLoggingLevel(LogLevel level) = 0;
  virtual void log(const std::string &message,
                   LogLevel level = LogLevel::Info,
                   const std::source_location &location =
                       std::source_location::current()) = 0;
};
