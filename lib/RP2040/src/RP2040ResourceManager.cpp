#include "../inc/RP2040ResourceManager.hpp"
#include "../inc/RP2040Resource.hpp"
#include <algorithm>
#include <cstring>
#include <pico/time.h>
#include <string>

extern "C"
{
#include "dhcp.h"
#include "wizchip_conf.h"
#include "wizchip_spi.h"
}

namespace
{
constexpr uint8_t kDhcpSocket{1};
constexpr uint16_t kDhcpBufferSize{1024};
constexpr uint8_t kDhcpTimeoutSeconds{15};

auto toWizNetInfo(
    const RP2040NetworkConfig &config) -> wiz_NetInfo
{
  wiz_NetInfo netInfo{};
  std::copy(config.mac.begin(), config.mac.end(), netInfo.mac);
  std::copy(config.ip.begin(), config.ip.end(), netInfo.ip);
  std::copy(config.subnetMask.begin(), config.subnetMask.end(), netInfo.sn);
  std::copy(config.gateway.begin(), config.gateway.end(), netInfo.gw);
  std::copy(config.dns.begin(), config.dns.end(), netInfo.dns);
  netInfo.dhcp = config.useDhcp ? NETINFO_DHCP : NETINFO_STATIC;
  return netInfo;
}

auto isZeroIpv4(
    const uint8_t ip[4]) -> bool
{
  return ip[0] == 0U && ip[1] == 0U && ip[2] == 0U && ip[3] == 0U;
}

auto runDhcp(
    LoggerIfc &logger,
    wiz_NetInfo &netInfo) -> bool
{
  static uint8_t dhcpBuffer[kDhcpBufferSize]{};

  DHCP_init(kDhcpSocket, dhcpBuffer);

  for (uint8_t elapsedSeconds = 0; elapsedSeconds < kDhcpTimeoutSeconds;
       ++elapsedSeconds)
  {
    DHCP_time_handler();
    const uint8_t state = DHCP_run();

    if (state == DHCP_IP_ASSIGN || state == DHCP_IP_CHANGED ||
        state == DHCP_IP_LEASED)
    {
      getIPfromDHCP(netInfo.ip);
      getGWfromDHCP(netInfo.gw);
      getSNfromDHCP(netInfo.sn);
      getDNSfromDHCP(netInfo.dns);
      netInfo.dhcp = NETINFO_DHCP;
      network_initialize(netInfo);
      logger.log("DHCP lease acquired");
      return true;
    }

    if (state == DHCP_FAILED || state == DHCP_STOPPED)
    {
      logger.log("DHCP failed during initialization", LogLevel::Error);
      return false;
    }

    sleep_ms(1000);
  }

  logger.log("DHCP timed out", LogLevel::Error);
  return false;
}

auto initializeW5500(
    LoggerIfc &logger,
    const RP2040NetworkConfig &networkConfig) -> bool
{
  wizchip_spi_initialize();
  wizchip_cris_initialize();
  wizchip_reset();
  wizchip_initialize();
  wizchip_check();

  wiz_NetInfo requestedNetInfo = toWizNetInfo(networkConfig);
  if (requestedNetInfo.dhcp == NETINFO_DHCP)
  {
    if (!runDhcp(logger, requestedNetInfo))
    {
      return false;
    }
  }
  else
  {
    network_initialize(requestedNetInfo);
  }

  print_network_information(requestedNetInfo);

  wiz_NetInfo appliedNetInfo{};
  wizchip_getnetinfo(&appliedNetInfo);

  if (requestedNetInfo.dhcp == NETINFO_DHCP)
  {
    if (isZeroIpv4(appliedNetInfo.ip))
    {
      logger.log("W5500 init failed: DHCP returned 0.0.0.0", LogLevel::Error);
      return false;
    }
  }
  else if (std::memcmp(appliedNetInfo.ip,
                       requestedNetInfo.ip,
                       sizeof(requestedNetInfo.ip)) != 0)
  {
    logger.log("W5500 init failed: applied IP does not match requested IP",
               LogLevel::Error);
    return false;
  }

  logger.log("W5500 init completed");
  return true;
}
} // namespace

auto RP2040ResourceManager::create(
    LoggerIfc &logger,
    const RP2040NetworkConfig &networkConfig)
    -> std::unique_ptr<RP2040ResourceManager>
{
  auto manager = std::unique_ptr<RP2040ResourceManager>(
      new RP2040ResourceManager(logger));

  manager->isNetworkInitialized_ = initializeW5500(logger, networkConfig);
  if (!manager->isNetworkInitialized_)
  {
    return nullptr;
  }

  return manager;
}

RP2040ResourceManager::RP2040ResourceManager(
    LoggerIfc &logger)
    : logger_(logger)
{
}

auto RP2040ResourceManager::openResource(
    const std::string &resourceString) -> std::unique_ptr<ResourceIfc>
{
  if (!isNetworkInitialized_)
  {
    logger_.log("W5500/network stack is not initialized", LogLevel::Error);
    return nullptr;
  }

  return RP2040Resource::create(logger_, resourceString);
}
