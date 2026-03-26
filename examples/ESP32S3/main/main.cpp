#include "ESP32/inc/ESP32Logger.hpp"
#include "ESP32/inc/ESP32ResourceManager.hpp"

extern "C" void app_main(
    void)
{
  ESP32Logger logger;
  logger.setLoggingLevel(LogLevel::Debug);

  ESP32ResourceManager manager(logger);
  logger.log("ESP32S3 example using InstrumentControl ESP32 backend");

  auto resource = manager.openResource("UART0");
  if (resource != nullptr)
  {
    static_cast<void>(resource->query("*IDN?"));
  }
}
