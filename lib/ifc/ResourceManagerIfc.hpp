#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ResourceIfc.hpp"

class ResourceManagerIfc
{
public:
  virtual ~ResourceManagerIfc() = default;

  [[nodiscard]] virtual auto listAvailableResources() const
      -> std::vector<std::string> = 0;
  virtual auto openResource(const std::string &resourceString)
      -> std::unique_ptr<ResourceIfc> = 0;
};
