#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

enum class ResourceAddressParseError : uint8_t
{
  MissingColon,
  InvalidIp,
  EmptyPort,
  InvalidPort
};

struct ParsedResourceAddress
{
  std::array<uint8_t, 4> ipOctets{};
  std::string ipString;
  uint16_t port{0};
};

struct ResourceAddressParseResult
{
  bool isOk{false};
  ResourceAddressParseError error{ResourceAddressParseError::InvalidIp};
  ParsedResourceAddress value{};
};

inline auto parseIpv4Octets(
    std::string_view ipString,
    std::array<uint8_t,
               4> &outIp) -> bool
{
  if (ipString.empty())
  {
    return false;
  }

  size_t octetIndex = 0;
  uint16_t octetValue = 0;
  size_t octetDigits = 0;
  constexpr uint16_t kDecimalBase = 10U;

  for (size_t i = 0; i <= ipString.size(); ++i)
  {
    const char currentChar = (i == ipString.size()) ? '.' : ipString[i];
    if (currentChar >= '0' && currentChar <= '9')
    {
      octetValue =
          static_cast<uint16_t>((octetValue * kDecimalBase) +
                                static_cast<uint16_t>(currentChar - '0'));
      ++octetDigits;
      if (octetDigits > 3U || octetValue > std::numeric_limits<uint8_t>::max())
      {
        return false;
      }
      continue;
    }

    if (currentChar != '.' || octetDigits == 0U || octetIndex >= outIp.size())
    {
      return false;
    }

    outIp[octetIndex] = static_cast<uint8_t>(octetValue);
    ++octetIndex;
    octetValue = 0;
    octetDigits = 0;
  }

  return octetIndex == outIp.size();
}

inline auto parsePortString(
    std::string_view portString,
    uint16_t &outPort) -> bool
{
  if (portString.empty())
  {
    return false;
  }

  uint32_t parsedPort = 0;
  constexpr uint32_t kDecimalBase = 10U;
  for (const char currentChar : portString)
  {
    if (currentChar < '0' || currentChar > '9')
    {
      return false;
    }
    parsedPort =
        (parsedPort * kDecimalBase) + static_cast<uint32_t>(currentChar - '0');
    if (parsedPort > std::numeric_limits<uint16_t>::max())
    {
      return false;
    }
  }

  outPort = static_cast<uint16_t>(parsedPort);
  return true;
}

inline auto parseResourceAddress(
    std::string_view resourceString) -> ResourceAddressParseResult
{
  ResourceAddressParseResult result{};

  const size_t colonPos = resourceString.find(':');
  if (colonPos == std::string_view::npos)
  {
    result.error = ResourceAddressParseError::MissingColon;
    return result;
  }

  const std::string_view ipView = resourceString.substr(0, colonPos);
  const std::string_view portView = resourceString.substr(colonPos + 1);

  if (!parseIpv4Octets(ipView, result.value.ipOctets))
  {
    result.error = ResourceAddressParseError::InvalidIp;
    return result;
  }

  if (portView.empty())
  {
    result.error = ResourceAddressParseError::EmptyPort;
    return result;
  }

  if (!parsePortString(portView, result.value.port))
  {
    result.error = ResourceAddressParseError::InvalidPort;
    return result;
  }

  result.value.ipString = std::string(ipView);
  result.isOk = true;
  return result;
}
