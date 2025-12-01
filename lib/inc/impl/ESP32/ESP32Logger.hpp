#pragma once
#include "ifc/LoggerIfc.hpp"
#include <print>
#include <source_location>
#include <string>

class ESP32Logger : public LoggerIfc
{
public:
  void log(
      const std::string &message,
      LogLevel level = LogLevel::Info,
      const std::source_location &location =
          std::source_location::current()) override
  {
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

    default:
      std::println("UNKNOWN LOG LEVEL: {}", message);
      break;
    }
  }
};
