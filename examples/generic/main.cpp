#include "ifc/LoggerIfc.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <string>
#ifdef IMPLEMENTATION_VISA
#include "impl/ConsoleLogger.hpp"
#include "impl/PCDataLogger.hpp"
#include "impl/VISA/VISAResourceManager.hpp"
using Logger = ConsoleLogger;
using DataLogger = PCDataLogger;
using ResourceManager = VISAResourceManager;

#elifdef IMPLEMENTATION_VISAClient
#include "impl/ConsoleLogger.hpp"
#include "impl/PCDataLogger.hpp"
#include "impl/VISAClient/VISAClientResourceManager.hpp"
using Logger = ConsoleLogger;
using DataLogger = PCDataLogger;
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

constexpr uint16_t DATALOGGER_BUFFER_SIZE = 1024;
// example commands to be repeated in communications loop
constexpr auto SCPI_COMMANDS = {"*IDN?", "*RST", ":AUTOSET EXECUTE"};

auto main() -> int
{
  // create logger and enable highest level of logs
  Logger logger;
  logger.setLoggingLevel(LogLevel::Trace);

  // create datalogger
  DataLogger dataLogger(DATALOGGER_BUFFER_SIZE);

  ResourceManager manager(logger);

  logger.log("Example: Resource manager instantiated");

  auto resources = manager.listAvailableResources();
  std::string resourcesLog{};
  for (const auto &resource : resources)
  {
    resourcesLog += '\t' + resource + '\n';
  }
  logger.log(
      fmt::format("Available resources:\n{}\nsize of resources vector: {}",
                  resourcesLog,
                  resources.size()));

  auto resource =
      manager.openResource("TCPIP::10.153.1.20::INSTR"); // example IP string
  if (resource != nullptr)
  {
    for (const auto &command : SCPI_COMMANDS)
    {
      // TODO: make this more robust, also differentating between write and
      // query doesn't work, only writes occur now
      auto last_char_idx = (sizeof(*command) / sizeof(command[0])) - 1;
      if (command[last_char_idx] == '?')
      {
        dataLogger.log("query," + std::string(command) + "," +
                       resource->query(command).value);
      }
      else
      {
        dataLogger.log("write," + std::string(command) + "," + '-');
        resource->write(command);
      }
    }
  }

  return 0;
}
