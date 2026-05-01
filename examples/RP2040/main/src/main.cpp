#include "../../lib/RP2040/inc/RP2040Logger.hpp"
#include "../../lib/RP2040/inc/RP2040ResourceManager.hpp"
#include "InputHelpers.hpp"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "ifc/LoggerIfc.hpp"
#include "network_settings.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <atomic>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
// LED define
#define LED_PIN 25

static repeating_timer g_heartbeat_timer;
static repeating_timer g_monitor_timer;
static std::atomic<bool> g_log_now{false};
static std::atomic<bool> g_busy{false};
static std::atomic<uint32_t> g_total_ticks{0};
static std::atomic<uint32_t> g_busy_ticks{0};
static std::atomic<uint32_t> g_ms_count{0};
static std::atomic<uint32_t> g_last_total{0};
static std::atomic<uint32_t> g_last_busy{0};
static std::atomic<uint32_t> g_window_min_free{UINT32_MAX};
static std::atomic<uint32_t> g_last_min_free{0};
static size_t g_heap_total = 0;

// forward declaration so the timer callback can call it
static auto get_free_ram_bytes() -> size_t;

// monitor timer callback: runs every 1 ms, accumulates samples
static auto monitor_timer_callback(
    repeating_timer *t) -> bool
{
  (void)t;
  g_total_ticks.fetch_add(1, std::memory_order_relaxed);
  if (g_busy.load(std::memory_order_relaxed))
    g_busy_ticks.fetch_add(1, std::memory_order_relaxed);
  // update window minimum free RAM (cheap sbrk-based check)
  size_t free_now = get_free_ram_bytes();
  uint32_t prev_min = g_window_min_free.load(std::memory_order_relaxed);
  if (free_now < prev_min)
    g_window_min_free.store((uint32_t)free_now, std::memory_order_relaxed);
  g_ms_count.fetch_add(1, std::memory_order_relaxed);
  if (g_ms_count.load(std::memory_order_relaxed) >= 1000)
  {
    // atomically snapshot and reset counters using exchange to avoid races
    uint32_t total = g_total_ticks.exchange(0, std::memory_order_relaxed);
    uint32_t busy = g_busy_ticks.exchange(0, std::memory_order_relaxed);
    uint32_t win_min =
        g_window_min_free.exchange(UINT32_MAX, std::memory_order_relaxed);
    // store snapshots for main loop to consume
    g_last_total.store(total, std::memory_order_relaxed);
    g_last_busy.store(busy, std::memory_order_relaxed);
    g_last_min_free.store(win_min == UINT32_MAX ? 0u : win_min,
                          std::memory_order_relaxed);
    g_ms_count.store(0, std::memory_order_relaxed);
    g_log_now.store(true, std::memory_order_relaxed);
  }
  return true;
}

static auto get_free_ram_bytes() -> size_t
{
  void *heap_end = sbrk(0);
  volatile char stack_var;
  uintptr_t sp = (uintptr_t)&stack_var;
  uintptr_t heap = (uintptr_t)heap_end;
  return sp > heap ? (size_t)(sp - heap) : 0;
}

auto timer_heartbeat_callback(
    repeating_timer *timer) -> bool
{
  (void)timer;
  gpio_put(LED_PIN, !gpio_get(LED_PIN));
  return true;
}

auto main() -> int
{
  // Set max clock speed: 133MHz for RP2040
  bool clock_ok = set_sys_clock_khz(133000, true);
  if (!clock_ok)
  {
    while (true)
    {
      tight_loop_contents();
    }
  }

  stdio_init_all();
  sleep_ms(3000);

  // approximate total heap available: measure once early (stack - initial heap
  // end)
  {
    void *heap_end_init = sbrk(0);
    volatile char stack_var_init;
    uintptr_t sp_init = (uintptr_t)&stack_var_init;
    uintptr_t heap_init = (uintptr_t)heap_end_init;
    g_heap_total = sp_init > heap_init ? (size_t)(sp_init - heap_init) : 0;
  }

  // LED initialisation
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  // Heartbeat - toggle LED every 500ms
  add_repeating_timer_ms(
      500, timer_heartbeat_callback, nullptr, &g_heartbeat_timer);

  RP2040Logger logger;
  logger.setLoggingLevel(LogLevel::Info);

  const RP2040NetworkConfig networkConfig{
      .mac = NETWORK_MAC,
      .ip = NETWORK_IP,
      .subnetMask = NETWORK_SUBNET,
      .gateway = NETWORK_GATEWAY,
      .dns = NETWORK_DNS,
      .useDhcp = NETWORK_USE_DHCP,
  };

  auto manager = RP2040ResourceManager::create(logger, networkConfig);
  if (manager == nullptr)
  {
    logger.log("Failed to initialize RP2040ResourceManager", LogLevel::Error);
    while (true)
    {
      tight_loop_contents();
    }
  }

  logger.log("RP2040 example using InstrumentControl RP2040 backend");

  auto resourceString =
      example_input::readLine("Enter VISA resource string: ");
  if (resourceString.empty())
  {
    logger.log("ERROR: resource string is empty, going into infinite loop");
    while (true)
    {
      tight_loop_contents();
    }
  }

  auto resource = manager->openResource(resourceString);

  if (resource == nullptr)
  {
    logger.log("ERROR: resource is a nullptr, going into infinite loop");
    while (true)
    {
    }
  }

  auto commandPlan = example_input::readCommandPlan();

  // start monitor timer: sample every 1 ms, aggregate to 1s windows
  add_repeating_timer_ms(1, monitor_timer_callback, nullptr, &g_monitor_timer);

  while (true)
  {

    for (const auto &step : commandPlan)
    {
      g_busy.store(true, std::memory_order_relaxed);
      step.execute(*resource, step.command);
      g_busy.store(false, std::memory_order_relaxed);
    }

    if (g_log_now)
    {
      g_log_now.store(false, std::memory_order_relaxed);
      uint64_t ts = to_us_since_boot(get_absolute_time());
      size_t free_ram = get_free_ram_bytes();
      // no PSRAM on RP2040 (field removed from JSON)
      // compute cpu percent from sampled counters
      unsigned tot = g_last_total.load(std::memory_order_relaxed);
      unsigned busy = g_last_busy.load(std::memory_order_relaxed);
      double cpu_pct = -1.0;
      if (tot > 0)
        cpu_pct = 100.0 * ((double)busy / (double)tot);

      // min_free from window (last 1s)
      unsigned min_free = g_last_min_free.load(std::memory_order_relaxed);

      if (cpu_pct >= 0.0)
        printf("{\"ts_us\":%llu,\"plat\":\"RP2040\",\"free_heap\":%u,\"heap_"
               "total\":%u,\"min_free\":%u,\"cpu_pct\":%.2f}\n",
               (unsigned long long)ts,
               (unsigned)free_ram,
               (unsigned)g_heap_total,
               (unsigned)min_free,
               cpu_pct);
      else
        printf("{\"ts_us\":%llu,\"plat\":\"RP2040\",\"free_heap\":%u,\"heap_"
               "total\":%u,\"min_free\":%u,\"cpu_pct\":null}\n",
               (unsigned long long)ts,
               (unsigned)free_ram,
               (unsigned)g_heap_total,
               (unsigned)min_free);
    }
  }

  return 0;
}
