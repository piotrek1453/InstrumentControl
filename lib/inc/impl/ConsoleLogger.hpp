#pragma once
#include "ifc/LoggerIfc.hpp"
#include "spdlog/spdlog.h"
#include <string_view>

class ConsoleLogger : public LoggerIfc
{
public:
  void log(std::string_view message, LogLevel level = LogLevel::Info) override
  {
    switch (level)
    {
    case LogLevel::Info:
      spdlog::info("{}", message);
      break;

    case LogLevel::Warn:
      spdlog::warn("{}", message);
      break;

    case LogLevel::Error:
      spdlog::error("{}", message);
      break;

    case LogLevel::Debug:
      spdlog::debug("{}", message);
      break;

    default:
      spdlog::warn("UNKNOWN LOG LEVEL:\n{}", message);
      break;
    }
  }
};
