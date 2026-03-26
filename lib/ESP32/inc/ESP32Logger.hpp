#pragma once
#include "../../ifc/LoggerIfc.hpp"
#include <print>
#include <source_location>
#include <string>

class ESP32Logger : public LoggerIfc
{
public:
  void setLoggingLevel(
      LogLevel level) override
  {
    mMinLogLevel = level;
  }

  void log(
      const std::string &message,
      LogLevel level = LogLevel::Info,
      const std::source_location &location =
          std::source_location::current()) override
  {
    static_cast<void>(location);
    if (static_cast<uint32_t>(level) < static_cast<uint32_t>(mMinLogLevel))
    {
      return;
    }

    switch (level)
    {
    case LogLevel::Info:
      std::println("INFO: {}", message);
      break;

    case LogLevel::Warn:
      std::println("WARN: {}", message);
      break;

    case LogLevel::Error:
      std::println("ERROR: {}", message);
      break;

    case LogLevel::Debug:
      std::println("DEBUG: {}", message);
      break;

    case LogLevel::Trace:
      std::println("TRACE: {}", message);
      break;

    default:
      std::println("UNKNOWN LOG LEVEL: {}", message);
      break;
    }
  }

private:
  LogLevel mMinLogLevel{LogLevel::Info};
};
