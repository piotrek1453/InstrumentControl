#pragma once
#include <cstdint>
#include <string_view>

enum class LogLevel : uint32_t { Info, Warn, Error, Debug, Trace };

class LoggerIfc {
public:
  virtual ~LoggerIfc() = default;

  virtual void setLoggingLevel(LogLevel level) = 0;
  virtual void log(std::string_view message,
                   LogLevel level = LogLevel::Info) = 0;
};
