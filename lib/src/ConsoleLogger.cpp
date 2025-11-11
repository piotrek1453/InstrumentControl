#include "impl/ConsoleLogger.hpp"
#include <spdlog/spdlog.h>

void ConsoleLogger::info(const std::string &msg) { spdlog::info(msg); }

void ConsoleLogger::warn(const std::string &msg) { spdlog::warn(msg); }

void ConsoleLogger::error(const std::string &msg) { spdlog::error(msg); }

void ConsoleLogger::debug(const std::string &msg) { spdlog::debug(msg); }
