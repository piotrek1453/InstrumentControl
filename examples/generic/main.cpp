#ifdef IMPLEMENTATION_VISA
#include "impl/ConsoleLogger.hpp"
#include "impl/VISA/VISAResourceManager.hpp"
typedef ConsoleLogger Logger;
typedef VISAResourceManager ResourceManager;
#endif

int main()
{
  Logger logger;
  ResourceManager manager(logger);

  logger.log("Example: Resource manager instantiated");

  manager.listAvailableResources();
  std::unique_ptr<ResourceIfc> resource = manager.openResource("test");

  resource->query("*IDN?");

  logger.log("InstrumentControl example built with implementation flag.");
  return 0;
}
