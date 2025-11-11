#pragma once
#include "LoggerIfc.hpp"

class ConsoleLogger : public LoggerIfc {
public:
  void info(const std::string &msg) override;
  void warn(const std::string &msg) override;
  void error(const std::string &msg) override;
  void debug(const std::string &msg) override;
};
