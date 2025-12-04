#pragma once
#include <string>

class DataLoggerIfc
{
public:
  virtual ~DataLoggerIfc() = default;

  virtual void log(const std::string &data) = 0;
};
