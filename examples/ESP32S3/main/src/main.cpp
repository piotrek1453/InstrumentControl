#include "ESP32/inc/ESP32Logger.hpp"
#include "ESP32/inc/ESP32ResourceManager.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi.hpp"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <esp_heap_caps.h>
#include <esp_timer.h>

// Unified monitoring: JSON lines with fields: ts_us, platform, free_heap,
// min_free, cpu_percent Sampling profiler: 1ms periodic timer increments
// samples; busy flag marks active work
static std::atomic_bool g_busy{false};
static std::atomic_uint g_total_samples{0};
static std::atomic_uint g_busy_samples{0};

static void sampling_timer_cb(
    void * /*arg*/)
{
  g_total_samples.fetch_add(1, std::memory_order_relaxed);
  if (g_busy.load(std::memory_order_relaxed))
    g_busy_samples.fetch_add(1, std::memory_order_relaxed);
}

static void monitor_task(
    void *)
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

extern "C" void vConfigureTimerForRunTimeStats(
    void)
{
  // no explicit timer required; esp_timer is used
}

extern "C" unsigned long ulGetRunTimeCounterValue(
    void)
{
  // return run-time counter in units of 0.1 ms
  return (unsigned long)(esp_timer_get_time() / 10);
}

extern "C" void app_main(
    void)
{
  ESP32Logger logger;
  logger.setLoggingLevel(LogLevel::Info);

  WiFi wifi(logger);
  wifi.wifi_config();

  ESP32ResourceManager manager(logger);
  logger.log("ESP32S3 example using InstrumentControl ESP32 backend");

  // start system monitor task
  xTaskCreate(
      monitor_task, "sysmon", 4096, nullptr, tskIDLE_PRIORITY + 1, nullptr);

  auto resource = manager.openResource(SERVER_IP_PORT_PAIR);

  while (true)
  {
    if (resource != nullptr)
    {
      g_busy.store(true, std::memory_order_relaxed);
      resource->query("*IDN?\r\n");
      g_busy.store(false, std::memory_order_relaxed);
    }
  }
}
