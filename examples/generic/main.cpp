#ifdef IMPLEMENTATION_VISA
#include "impl/ConsoleLogger.hpp"
#include "impl/VISA/VISAResourceManager.hpp"
using Logger = ConsoleLogger;
using ResourceManager = VISAResourceManager;

#elifdef IMPLEMENTATION_VISAClient
#include "impl/ConsoleLogger.hpp"
#include "impl/VISAClient/VISAClientResourceManager.hpp"
using Logger = ConsoleLogger;
using ResourceManager = VISAClientResourceManager;

#elifdef IMPLEMENTATION_ESP32
#include "impl/ESP32/ESP32Logger.hpp"
#include "impl/ESP32/ESP32ResourceManager.hpp"
using Logger = ESP32Logger;
using ResourceManager = ESP32ResourceManager;

#elifdef IMPLEMENTATION_RP2040
#include "impl/RP2040/RP2040Logger.hpp"
#include "impl/RP2040/RP2040ResourceManager.hpp"
using Logger = RP2040Logger;
using ResourceManager = RP2040ResourceManager;

#else
#error "Selected implementation does not provide headers here yet"
#endif

auto main() -> int {
  Logger logger;
  ResourceManager manager(logger);

  logger.log("Example: Resource manager instantiated");

  logger.log("Available resources:\n");
  for (const auto &resource : manager.listAvailableResources()) {
    logger.log(resource);
  }

  auto resource = manager.openResource("test");
  if (resource) {
    (void)resource->query("*IDN?");
  }

  logger.log("InstrumentControl example built with selected implementation.");
  return 0;
}
