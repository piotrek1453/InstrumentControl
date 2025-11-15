#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "ResourceIfc.hpp"

class ResourceManagerIfc
{
public:
  virtual ~ResourceManagerIfc() = default;

  virtual std::vector<std::string> listAvailableResources() const = 0;
  virtual std::unique_ptr<ResourceIfc> openResource(std::string_view resourceString) = 0;
};
