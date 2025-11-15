#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "ResourceIfc.hpp"

class ResourceManagerIfc {
public:
  virtual ~ResourceManagerIfc() = default;

  [[nodiscard]] virtual auto listAvailableResources() const
      -> std::vector<std::string> = 0;
  virtual auto openResource(std::string_view resourceString)
      -> std::unique_ptr<ResourceIfc> = 0;
};
