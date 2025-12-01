#pragma once
#include "ifc/LoggerIfc.hpp"
#include <print>
#include <string>

class RP2040Logger : public LoggerIfc
{
public:
  void log(
      const std::string &message,
      LogLevel level = LogLevel::Info) override
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
