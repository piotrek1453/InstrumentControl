#pragma once
#include "../../ifc/LoggerIfc.hpp"
#include <cstdio>
#include <source_location>
#include <string>
#include <sys/time.h>

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

    // generate microsecond-precision timestamp
    // code from
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/system_time.html
    gettimeofday(&tv_now, nullptr);
    auto timestamp =
        (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

// print full info in debug mode otherwise just log level and message
#if defined(NDEBUG)
    static_cast<void>(location);
    printf("(%llu) %s : %s\n",
           timestamp,
           logLevelToString(level),
           message.c_str());
#else
    printf("(%llu) %s [%s : %lu : %s] : %s\n",
           timestamp,
           logLevelToString(level),
           location.file_name(),
           static_cast<unsigned long>(location.line()),
           location.function_name(),
           message.c_str());
#endif
  }

private:
  LogLevel mLogLevel{LogLevel::Info};
  struct timeval tv_now;
};
