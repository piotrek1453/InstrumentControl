#include "ESP32/inc/ESP32Logger.hpp"
#include "ESP32/inc/ESP32ResourceManager.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(
    void)
{
  ESP32Logger logger;
  logger.setLoggingLevel(LogLevel::Trace);

  ESP32ResourceManager manager(logger);
  logger.log("ESP32S3 example using InstrumentControl ESP32 backend");

  for (int i = 0; i <= static_cast<int>(LogLevel::Trace); ++i)
  {
    logger.log(std::string("Logging at level ") +
               logLevelToString(static_cast<LogLevel>(i)));
  }

  auto resource = manager.openResource("UART0");
  if (resource != nullptr)
  {
    static_cast<void>(resource->query("*IDN?"));
  }

  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
