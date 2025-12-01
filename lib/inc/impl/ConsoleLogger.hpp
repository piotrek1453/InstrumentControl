#pragma once
#include "ifc/LoggerIfc.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <string>
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
  void setLoggingLevel(
      LogLevel level) override
  {
    mLogger->set_level(logLevelMap.at(level));
  }

  void log(
      const std::string &message,
      LogLevel level = LogLevel::Info) override
  {
    mLogger->log(logLevelMap.at(level), message);
  }

private:
  std::shared_ptr<spdlog::logger> mLogger{spdlog::stdout_color_mt("console")};
};
