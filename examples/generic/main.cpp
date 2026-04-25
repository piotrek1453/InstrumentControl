#include "ifc/LoggerIfc.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <string>
#include <string_view>
#ifdef IMPLEMENTATION_VISA
#include "VISA/inc/ConsoleLogger.hpp"
#include "VISA/inc/VISAResourceManager.hpp"
using Logger = ConsoleLogger;
using ResourceManager = VISAResourceManager;

#elifdef IMPLEMENTATION_VISAClient
#include "VISA/inc/ConsoleLogger.hpp"
#include "VISAClient/inc/VISAClientResourceManager.hpp"
using Logger = ConsoleLogger;
using ResourceManager = VISAClientResourceManager;

#elifdef IMPLEMENTATION_ESP32
#include "ESP32/inc/ESP32Logger.hpp"
#include "ESP32/inc/ESP32ResourceManager.hpp"
using Logger = ESP32Logger;
using ResourceManager = ESP32ResourceManager;

#elifdef IMPLEMENTATION_RP2040
#include "RP2040/inc/RP2040Logger.hpp"
#include "RP2040/inc/RP2040ResourceManager.hpp"
using Logger = RP2040Logger;
using ResourceManager = RP2040ResourceManager;

#else
#error "Selected implementation does not provide headers here yet"
#endif

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
      // TODO: make this more robust, also differentating between write and
      // query doesn't work, only writes occur now
      const auto commandView = std::string(command);
      if (commandView.find('?') != std::string::npos)
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
