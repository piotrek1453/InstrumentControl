#pragma once
#include "../../ifc/LoggerIfc.hpp"
#include <esp_log.h>
#include <source_location>
#include <string>

class ESP32Logger : public LoggerIfc
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

    const auto espLevel = toEspLogLevel(level);

// print full info in debug mode otherwise just log level and message
#if defined(NDEBUG)
    static_cast<void>(location);
    esp_log_write(
        espLevel, kTag, "%s : %s\n", logLevelToString(level), message.c_str());
#else
    esp_log_write(espLevel,
                  kTag,
                  "[%s : %lu : %s] %s : %s\n",
                  location.file_name(),
                  static_cast<unsigned long>(location.line()),
                  location.function_name(),
                  logLevelToString(level),
                  message.c_str());
#endif
  }

private:
  static constexpr auto toEspLogLevel(
      LogLevel level) -> esp_log_level_t
  {
    switch (level)
    {
    case LogLevel::Trace:
      return ESP_LOG_VERBOSE;
    case LogLevel::Debug:
      return ESP_LOG_DEBUG;
    case LogLevel::Info:
      return ESP_LOG_INFO;
    case LogLevel::Warn:
      return ESP_LOG_WARN;
    case LogLevel::Error:
      return ESP_LOG_ERROR;
    default:
      return ESP_LOG_INFO;
    }
  }

  static constexpr const char *kTag = "InstrumentControl";
  LogLevel mLogLevel{LogLevel::Info};
};
