#pragma once

#include <string>

struct ReadResult
{
  bool isOk = false;
  std::string value;

  static auto success(
      const std::string &view) -> ReadResult
  {
    return ReadResult{.isOk = true, .value = view};
  }

  static auto failure() -> ReadResult
  {
    return ReadResult{.isOk = false, .value = {}};
  }
};

class ResourceIfc
{
public:
  virtual ~ResourceIfc() = default;

  virtual auto write(const std::string &command) -> bool = 0;
  virtual auto read() -> ReadResult = 0;
  virtual auto query(const std::string &command) -> ReadResult = 0;
};
