#pragma once
#include "ifc/DataLoggerIfc.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>

class PCDataLogger : DataLoggerIfc
{
public:
  explicit PCDataLogger(
      std::filesystem::path csvPath) noexcept
      : mCsvFileHandle(csvPath)
  {
  }

  ~PCDataLogger() override
  {
    if (mCsvFileHandle.is_open())
    {
      mCsvFileHandle.flush();
      mCsvFileHandle.close();
    }
  }

  void log(
      const std::string &data) override
  {
    if (mCsvFileHandle.is_open())
    {
      mCsvFileHandle << ++mCurrentDataIndex << ',' << data << '\n';
      mCsvFileHandle.flush();
    }
  }

private:
  size_t mCurrentDataIndex{0};
  std::ofstream mCsvFileHandle;
};
