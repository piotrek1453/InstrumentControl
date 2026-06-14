#include "ESP32/inc/ESP32Logger.hpp"
#include "ESP32/inc/ESP32ResourceManager.hpp"
#include "InputHelpers.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi.hpp"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <driver/uart.h>
#include <esp_err.h>
#include <esp_heap_caps.h>
#include <esp_timer.h>

// Unified monitoring: JSON lines with fields: ts_us, platform, free_heap,
// min_free, cpu_percent Sampling profiler: 1ms periodic timer increments
// samples; busy flag marks active work
static std::atomic_bool g_busy{false};
static std::atomic_uint g_total_samples{0};
static std::atomic_uint g_busy_samples{0};

static auto sampling_timer_cb(
    void *ctx) -> void
{
  g_total_samples.fetch_add(1, std::memory_order_relaxed);
  if (g_busy.load(std::memory_order_relaxed))
    g_busy_samples.fetch_add(1, std::memory_order_relaxed);
}

static auto monitor_task(
    void *ctx) -> void
{
  // start esp_timer periodic 1ms sampler
  esp_timer_create_args_t args{};
  args.callback = &sampling_timer_cb;
  args.arg = nullptr;
  args.dispatch_method = ESP_TIMER_TASK;
  esp_timer_handle_t sampler = nullptr;
  esp_timer_create(&args, &sampler);
  esp_timer_start_periodic(sampler, 1000); // 1000 us = 1 ms

  for (;;)
  {
    int64_t ts = esp_timer_get_time();
    size_t free_heap = esp_get_free_heap_size();
    size_t min_free = esp_get_minimum_free_heap_size();
    size_t heap_total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    // no PSRAM reporting here

    // snapshot and reset counters for 1s window
    unsigned tot = g_total_samples.exchange(0, std::memory_order_relaxed);
    unsigned busy = g_busy_samples.exchange(0, std::memory_order_relaxed);
    double cpu_percent = -1.0;
    if (tot > 0)
      cpu_percent = 100.0 * ((double)busy / (double)tot);

    if (cpu_percent >= 0.0)
      printf("{\"ts_us\":%lld,\"plat\":\"ESP32\",\"free_heap\":%u,\"heap_"
             "total\":%u,\"min_free\":%u,\"cpu_pct\":%.2f}\n",
             (long long)ts,
             (unsigned)free_heap,
             (unsigned)heap_total,
             (unsigned)min_free,
             cpu_percent);
    else
      printf("{\"ts_us\":%lld,\"plat\":\"ESP32\",\"free_heap\":%u,\"heap_"
             "total\":%u,\"min_free\":%u,\"cpu_pct\":null}\n",
             (long long)ts,
             (unsigned)free_heap,
             (unsigned)heap_total,
             (unsigned)min_free);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

extern "C" auto vConfigureTimerForRunTimeStats(
    void) -> void
{
  // no explicit timer required; esp_timer is used
}

extern "C" auto ulGetRunTimeCounterValue(
    void) -> unsigned long
{
  // return run-time counter in units of 0.1 ms
  return (unsigned long)(esp_timer_get_time() / 10);
}

extern "C" auto app_main(
    void) -> void
{
  ESP32Logger logger;
  logger.setLoggingLevel(LogLevel::Info);

  // bidirectional UART setup
  const uart_port_t consoleUart =
      static_cast<uart_port_t>(CONFIG_ESP_CONSOLE_UART_NUM);
  ESP_ERROR_CHECK(uart_driver_install(consoleUart, 1024, 0, 0, nullptr, 0));

  WiFi wifi(logger);
  wifi.wifi_config();

  ESP32ResourceManager manager(logger);
  logger.log("ESP32S3 example using InstrumentControl ESP32 backend");

  auto resourceString =
      example_input::readLine("Enter VISA resource string: ");
  if (resourceString.empty())
  {
    logger.log("ERROR: resource string is empty, going into infinite loop");
    while (true)
    {
      vTaskDelay(portMAX_DELAY);
    }
  }

  auto resource = manager.openResource(resourceString);

  if (resource == nullptr)
  {
    logger.log("ERROR: resource is a nullptr, going into infinite loop");
    while (true)
    {
      vTaskDelay(portMAX_DELAY);
    }
  }

  // resource comms example: read commands from CLI, decide whether they're a
  // write or query and execute in infinite loop

  // commands to be executed once, initialization
  std::printf("Initialization SCPI commands, executed only once at startup\n");
  auto commandPlanInit = example_input::readCommandPlan();
  // commands to be executed in loop
  std::printf("Repeated SCPI commands, executed in an infinite loop\n");
  auto commandPlanRepeat = example_input::readCommandPlan();

  // start system monitor task
  xTaskCreate(
      monitor_task, "sysmon", 4096, nullptr, tskIDLE_PRIORITY + 1, nullptr);

  // execute init commands
  for (const auto &step : commandPlanInit)
  {
    step.execute(*resource, step.command);
  }

  while (true)
  {
    // execute looped commands
    for (const auto &step : commandPlanRepeat)
    {
      g_busy.store(true, std::memory_order_relaxed);
      step.execute(*resource, step.command);
      g_busy.store(false, std::memory_order_relaxed);
    }
  }
}
