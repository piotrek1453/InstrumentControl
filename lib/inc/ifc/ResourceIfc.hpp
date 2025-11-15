#pragma once

#include <string_view>

struct ReadResult
{
  bool ok = false;
  std::string value;

  static ReadResult success(std::string v)
  {
    return {true, std::move(v)};
  }

  static ReadResult failure()
  {
    return {false, {}};
  }
};

class ResourceIfc
{
public:
  virtual ~ResourceIfc() = default;

  virtual bool write(std::string_view command) = 0;
  virtual ReadResult read() = 0;
  virtual ReadResult query(std::string_view command) = 0;
};
