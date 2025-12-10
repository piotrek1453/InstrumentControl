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
      size_t bufferSize = 0,
      std::filesystem::path csvPath = std::filesystem::temp_directory_path() /
                                      "example_data.csv") noexcept
      : mBufferSize(bufferSize),
        mCsvFileHandle(csvPath)
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
      if (mCurrentDataIndex >= mBufferSize - 1)
      {
        mCsvFileHandle.flush();
      }
    }
  }

private:
  size_t mCurrentDataIndex{0};
  size_t mBufferSize{0};
  std::ofstream mCsvFileHandle;
};
