#pragma once

#include "ESP32Logger.hpp"
#include "ifc/LoggerIfc.hpp"
#include "network_settings.h"
#include <esp_event.h>
#include <freertos/event_groups.h>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

class ESP32Logger;

class WiFi
{
public:
  WiFi(
      LoggerIfc &logger)
      : mLogger(logger)
  {
  }
  ~WiFi() = default;

  auto wifi_config() -> void;

private:
  static auto wifi_event_handler(void *arg,
                                 esp_event_base_t event_base,
                                 int32_t event_id,
                                 void *event_data) -> void;
  auto handle_wifi_event(esp_event_base_t event_base,
                         int32_t event_id,
                         void *event_data) -> void;

  // WiFi configuration
  static constexpr char SSID[] = WIFI_SSID;
  static constexpr char PASSWORD[] = WIFI_PASSWORD;

  LoggerIfc &mLogger;
  EventGroupHandle_t wifi_event_group_ = nullptr;
};
