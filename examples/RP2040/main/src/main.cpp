#include "../../lib/RP2040/inc/RP2040Logger.hpp"
#include "../../lib/RP2040/inc/RP2040ResourceManager.hpp"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "ifc/LoggerIfc.hpp"
#include "network_settings.h"
#include "pico/stdlib.h"

#define LED_PIN 25

static repeating_timer g_heartbeat_timer;

auto timer_heartbeat_callback(
    repeating_timer *timer) -> bool
{
  (void)timer;
  gpio_put(LED_PIN, !gpio_get(LED_PIN));
  return true;
}

auto main() -> int
{
  bool clock_ok = set_sys_clock_khz(133000, true);
  if (!clock_ok)
  {
    while (true)
    {
      tight_loop_contents();
    }
  }

  stdio_init_all();

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  add_repeating_timer_ms(
      500, timer_heartbeat_callback, nullptr, &g_heartbeat_timer);

  RP2040Logger logger;
  logger.setLoggingLevel(LogLevel::Trace);

  RP2040ResourceManager manager(logger);
  logger.log("RP2040 example using InstrumentControl RP2040 backend");

  auto resource = manager.openResource(SERVER_IP_PORT_PAIR);

  while (true)
  {
    if (resource != nullptr)
    {
      resource->query("*IDN?\r\n");
    }
  }

  return 0;
}
