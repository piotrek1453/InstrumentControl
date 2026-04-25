#pragma once
#include "ifc/LoggerIfc.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/stdout_sinks.h"
#include <filesystem>
#include <fmt/format.h>
#include <memory>
#include <source_location>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <string>
#include <sys/time.h>
#include <unordered_map>

static const std::unordered_map<LogLevel, spdlog::level::level_enum>
    logLevelMap{{LogLevel::Info, spdlog::level::info},
                {LogLevel::Warn, spdlog::level::warn},
                {LogLevel::Error, spdlog::level::err},
                {LogLevel::Debug, spdlog::level::debug},
                {LogLevel::Trace, spdlog::level::trace}};

class ConsoleLogger : public LoggerIfc
{
public:
  ConsoleLogger()
  {
    if (mLogger)
    {
      mLogger->set_pattern("%v");
    }
  }
  void setLoggingLevel(
      LogLevel level) override
  {
    mLogger->set_level(logLevelMap.at(level));
  }

  void log(
      const std::string &message,
      LogLevel level = LogLevel::Info,
      const std::source_location &location =
          std::source_location::current()) override
  {
    auto filename =
        std::filesystem::path(location.file_name()).filename().string();

    struct timeval tv_now;
    gettimeofday(&tv_now, nullptr);
    unsigned long long timestamp =
        (unsigned long long)tv_now.tv_sec * 1000000ULL +
        (unsigned long long)tv_now.tv_usec;

#if defined(NDEBUG)
    static_cast<void>(location);
    auto formatted = fmt::format(
        "({}) {} : {}", timestamp, logLevelToString(level), message);
#else
    auto formatted = fmt::format("({}) {} [{} : {} : {}] : {}",
                                 timestamp,
                                 logLevelToString(level),
                                 filename,
                                 location.line(),
                                 location.function_name(),
                                 message);
#endif

    mLogger->log(logLevelMap.at(level), formatted);
  }

private:
  std::shared_ptr<spdlog::logger> mLogger{spdlog::stdout_color_mt("console")};
};
