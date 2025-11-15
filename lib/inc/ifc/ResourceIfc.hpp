#pragma once

#include <string_view>

struct ReadResult {
  bool ok = false;
  std::string_view value;

  static auto success(std::string_view view) -> ReadResult {
    return ReadResult{.ok = true, .value = std::move(view)};
  }

  static auto failure() -> ReadResult {
    return ReadResult{.ok = false, .value = {}};
  }
};

class ResourceIfc {
public:
  virtual ~ResourceIfc() = default;

  virtual auto write(std::string_view command) -> bool = 0;
  virtual auto read() -> ReadResult = 0;
  virtual auto query(std::string_view command) -> ReadResult = 0;
};
