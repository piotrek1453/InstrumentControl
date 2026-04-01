#include "ESP32/inc/ESP32Logger.hpp"
#include "ESP32/inc/ESP32ResourceManager.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi.hpp"

extern "C" void app_main(
    void)
{
  ESP32Logger logger;
  logger.setLoggingLevel(LogLevel::Trace);

  WiFi wifi(logger);
  wifi.wifi_config();

  ESP32ResourceManager manager(logger);
  logger.log("ESP32S3 example using InstrumentControl ESP32 backend");

  auto resource = manager.openResource(SERVER_IP_PORT_PAIR);
  if (resource != nullptr)
  {
    static_cast<void>(resource->write("*IDN?"));
  }

  while (true)
  {
    if (resource != nullptr)
    {
      auto readResult = resource->read();
    }
  }
}