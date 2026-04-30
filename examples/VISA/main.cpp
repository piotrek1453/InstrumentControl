#include "VISA/inc/ConsoleLogger.hpp"
#include "VISA/inc/VISAResourceManager.hpp"
#include "ifc/LoggerIfc.hpp"
#include <fmt/format.h>
#include <string>
#include <string_view>

using Logger = ConsoleLogger;
using ResourceManager = VISAResourceManager;

// example commands to be repeated in communications loop
constexpr auto SCPI_COMMANDS = {"*IDN?\r\n",
                                "*RST\r\n",
                                ":AUTOSET EXECUTE\r\n"};

auto main() -> int
{
  // create logger and enable highest level of logs
  Logger logger;
  logger.setLoggingLevel(LogLevel::Info);

  ResourceManager manager(logger);

  logger.log("Example: Resource manager instantiated");

  auto resource =
      manager.openResource("TCPIP::10.153.1.20::INSTR"); // example IP string
  if (resource != nullptr)
  {
    for (const auto &command : SCPI_COMMANDS)
    {
      if (std::string_view(command).find('?') != std::string_view::npos)
      {
        resource->query(command);
      }
      else
      {
        resource->write(command);
      }
    }
  }

  return 0;
}
