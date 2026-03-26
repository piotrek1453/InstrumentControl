#include "ifc/LoggerIfc.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <string>
#ifdef IMPLEMENTATION_VISA
#include "VISA/inc/ConsoleLogger.hpp"
#include "VISA/inc/PCDataLogger.hpp"
#include "VISA/inc/VISAResourceManager.hpp"
using Logger = ConsoleLogger;
using DataLogger = PCDataLogger;
using ResourceManager = VISAResourceManager;

#elif defined(IMPLEMENTATION_VISAClient)
#include "VISA/inc/ConsoleLogger.hpp"
#include "VISA/inc/PCDataLogger.hpp"
#include "VISAClient/inc/VISAClientResourceManager.hpp"
using Logger = ConsoleLogger;
using DataLogger = PCDataLogger;
using ResourceManager = VISAClientResourceManager;

#elif defined(IMPLEMENTATION_ESP32)
#include "ESP32/inc/ESP32Logger.hpp"
#include "ESP32/inc/ESP32ResourceManager.hpp"
using Logger = ESP32Logger;
using ResourceManager = ESP32ResourceManager;

#elif defined(IMPLEMENTATION_RP2040)
#include "RP2040/inc/RP2040Logger.hpp"
#include "RP2040/inc/RP2040ResourceManager.hpp"
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

#if defined(IMPLEMENTATION_VISA) || defined(IMPLEMENTATION_VISAClient)
  // create datalogger
  DataLogger dataLogger(DATALOGGER_BUFFER_SIZE);
#endif

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
#if defined(IMPLEMENTATION_VISA) || defined(IMPLEMENTATION_VISAClient)
        dataLogger.log("query," + std::string(command) + "," +
                       resource->query(command).value);
#else
        static_cast<void>(resource->query(command));
#endif
      }
      else
      {
#if defined(IMPLEMENTATION_VISA) || defined(IMPLEMENTATION_VISAClient)
        dataLogger.log("write," + std::string(command) + "," + '-');
#endif
        resource->write(command);
      }
    }
  }

  return 0;
}
