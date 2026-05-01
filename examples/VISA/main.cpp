#include "InputHelpers.hpp"
#include "VISA/inc/ConsoleLogger.hpp"
#include "VISA/inc/VISAResourceManager.hpp"
#include "ifc/LoggerIfc.hpp"
#include <fmt/format.h>

using Logger = ConsoleLogger;
using ResourceManager = VISAResourceManager;

auto main() -> int
{
  // create logger and enable highest level of logs
  Logger logger;
  logger.setLoggingLevel(LogLevel::Info);

  ResourceManager manager(logger);

  logger.log("Example: VISA resource manager instantiated");

  // resource enumeration example
  for (const auto &resource : manager.listAvailableResources())
  {
    logger.log(fmt::format("Found resource: {}", resource));
  }

  auto resourceString =
      example_input::readLine("Enter VISA resource string: ");
  if (resourceString.empty())
  {
    logger.log("ERROR: resource string is empty, terminating");
    return 1;
  }

  // resource opening example
  auto resource = manager.openResource(resourceString);

  if (resource == nullptr)
  {
    logger.log("ERROR: resource is a nullptr, terminating");
    return 1;
  }

  // resource comms example: read commands from CLI, decide whether they're a
  // write or query and execute in infinite loop
  auto commandPlan = example_input::readCommandPlan();
  while (true)
  {
    for (const auto &step : commandPlan)
    {
      step.execute(*resource, step.command);
    }
  }

  return 0;
}
