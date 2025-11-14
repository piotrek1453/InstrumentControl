#pragma once
#include "ifc/LoggerIfc.hpp"
#include "spdlog/spdlog.h"
#include <string_view>

class ConsoleLogger : public LoggerIfc {
public:
  void info(std::string_view msg) override { spdlog::info("{}", msg); }
  void warn(std::string_view msg) override { spdlog::warn("{}", msg); }
  void error(std::string_view msg) override { spdlog::error("{}", msg); }
  void debug(std::string_view msg) override { spdlog::debug("{}", msg); }
};
