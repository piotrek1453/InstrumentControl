#pragma once
#include "../../ifc/LoggerIfc.hpp"
#include <cstdio>
#include <source_location>
#include <string>

class RP2040Logger : public LoggerIfc
{
public:
  void setLoggingLevel(
      LogLevel level) override
  {
    mLogLevel = level;
  }

  void log(
      const std::string &message,
      LogLevel level = LogLevel::Info,
      const std::source_location &location =
          std::source_location::current()) override
  {
    if (static_cast<uint32_t>(level) > static_cast<uint32_t>(mLogLevel))
    {
      return;
    }

// print full info in debug mode otherwise just log level and message
#if defined(NDEBUG)
    static_cast<void>(location);
    printf("%s : %s\n", logLevelToString(level), message.c_str());
#else
    printf("[%s : %lu : %s] %s : %s\n",
           location.file_name(),
           static_cast<unsigned long>(location.line()),
           location.function_name(),
           logLevelToString(level),
           message.c_str());
#endif
  }

private:
  LogLevel mLogLevel{LogLevel::Info};
};
