#include "wifi.hpp"
#include "ifc/LoggerIfc.hpp"
#include <esp_err.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

auto WiFi::wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data) -> void
{
  auto *self = static_cast<WiFi *>(arg);
  if (self == nullptr)
  {
    return;
  }

  self->handle_wifi_event(event_base, event_id, event_data);
}

auto WiFi::handle_wifi_event(
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data) -> void
{
  static int retry_count = 0;
  const int MAX_RETRY = 5;

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    if (retry_count < MAX_RETRY)
    {
      mLogger.log("Disconnected, retrying... (" +
                  std::to_string(retry_count + 1) + "/" +
                  std::to_string(MAX_RETRY) + ")");
      esp_wifi_connect();
      retry_count++;
    }
    else
    {
      mLogger.log("Failed to connect after " + std::to_string(MAX_RETRY) +
                      " attempts",
                  LogLevel::Error);
      xEventGroupSetBits(wifi_event_group_, WIFI_FAIL_BIT);
    }
    xEventGroupClearBits(wifi_event_group_, WIFI_CONNECTED_BIT);
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    char ip_addr[16];
    snprintf(ip_addr, sizeof(ip_addr), IPSTR, IP2STR(&event->ip_info.ip));
    mLogger.log("Got IP: " + std::string(ip_addr));
    retry_count = 0; // counter reset
    xEventGroupSetBits(wifi_event_group_, WIFI_CONNECTED_BIT);
  }
}

auto WiFi::wifi_config() -> void
{
  // initialize non-volatile flash
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // initialize network stack
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  // initialize Wi-Fi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // event group
  wifi_event_group_ = xEventGroupCreate();
  esp_event_handler_instance_t instance_any_id, instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &wifi_event_handler,
                                                      this,
                                                      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &wifi_event_handler,
                                                      this,
                                                      &instance_got_ip));
  // station configuration
  wifi_config_t wifi_config = {};
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  strlcpy((char *)wifi_config.sta.ssid, SSID, sizeof(wifi_config.sta.ssid));
  strlcpy((char *)wifi_config.sta.password,
          PASSWORD,
          sizeof(wifi_config.sta.password));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  // connecting to network
  mLogger.log("connecting to network with SSID " + std::string(SSID) + "...");
  // wait max 20s for connection
  EventBits_t bits = xEventGroupWaitBits(wifi_event_group_,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         pdMS_TO_TICKS(20000));

  if (bits & WIFI_CONNECTED_BIT)
  {
    mLogger.log("WiFi connected");
  }
  else if (bits & WIFI_FAIL_BIT)
  {
    mLogger.log("WiFi connection failed", LogLevel::Error);
  }
  else
  {
    mLogger.log("Timeout connecting to WiFi", LogLevel::Error);
  }
}
