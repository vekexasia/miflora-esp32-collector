// #include "sdkconfig.h"
#include <Arduino.h>
// #include "esp_pm.h"
// #include "esp_sleep.h"
// #include "transmitters/_433.h"
#include "transmitters/mqttTransmitter.h"
#include "flora/flora.h"
#include "wifi/wifi.h"
#include "mqtt/mqtt.h"
#include "nvs_flash.h"
// #include "soc/soc.h"
// #include "soc/rtc_cntl_reg.h"
#include <Preferences.h>
// esp_pm_config_esp32_t pm_config = {
//     .max_freq_mhz = 80,         // e.g. 80, 160, 240
//     .min_freq_mhz = 40,         // e.g. 40
//     .light_sleep_enable = false, // enable light sleep
// };   
// boot count used to check if battery status should be read
// garden::_433 *my433;
garden::Flora *flora;
garden::WiFi *wifi;
garden::MQTT *mqtt;
garden::MQTTTransmitter *mqttTransmitter;

Preferences prefs;

void setup() {
  // nvs_flash_init();
  Serial.begin(115200);
  prefs.begin("floraData");

  wifi = new garden::WiFi();
  mqtt = new garden::MQTT();
  mqttTransmitter = new garden::MQTTTransmitter();
  flora = new garden::Flora(mqttTransmitter);

  wifi->setup();

  mqtt->setup();

  mqttTransmitter->setup();

  flora->setup();
  Serial.println("ZIOLUPO");


}

void loop() {
  // Try spooling old messages if any
  Serial.println("Loop");

  mqtt->loop();
  mqttTransmitter->spool();

  Serial.println("AfterSpool");
  // Collect new data.
  flora->loop();

  mqttTransmitter->teardown();
  mqtt->teardown();
  wifi->teardown();

  // Go to sleep.
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  // Serial.println("Waiting for restart");
  // delay(10 * 1000);
  // ESP.restart();
	esp_sleep_enable_timer_wakeup(((uint64_t) 10) * 1000000);
	esp_deep_sleep_start();

}