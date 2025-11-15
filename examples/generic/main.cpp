#ifdef IMPLEMENTATION_VISA
#include "impl/ConsoleLogger.hpp"
#include "impl/VISA/VISAResourceManager.hpp"
using Logger = ConsoleLogger;
using ResourceManager = VISAResourceManager;
#endif

auto main() -> int {
  Logger logger;
  ResourceManager manager(logger);

  logger.log("Example: Resource manager instantiated");

  logger.log("Available resources:\n");
  for (const auto &resource : manager.listAvailableResources()) {
    logger.log(resource);
  }
  std::unique_ptr<ResourceIfc> resource = manager.openResource("test");

  resource->query("*IDN?");

  logger.log("InstrumentControl example built with implementation flag.");
  return 0;
}
