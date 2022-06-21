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
TaskHandle_t restartTaskHandle = NULL;


void delayedRestart(void *parameter) {
  Serial.println("task started");
  delay(300000); // delay for five minutes
  Serial.println("Something got stuck, entering emergency hibernate...");
  ESP.restart();
}

void setup() {
  // nvs_flash_init();
  Serial.begin(115200);

  prefs.begin("floraData");

  wifi = new garden::WiFi();
  mqtt = new garden::MQTT();
  mqttTransmitter = new garden::MQTTTransmitter();
  flora = new garden::Flora(mqttTransmitter);

  
  Serial.println("ZIOLUPO");


}

void loop() {
  // Try spooling old messages if any
  Serial.println("Loop");
  xTaskCreate(delayedRestart, "restart", 4096, NULL, 1, &restartTaskHandle);

  wifi->setup();
  mqtt->setup();
  mqttTransmitter->setup();
  flora->setup();

  if (wifi->isConnected()) {
    mqtt->loop();
    mqttTransmitter->spool();
  }

  flora->loop();

  mqttTransmitter->teardown();
  mqtt->teardown();
  wifi->teardown();
  // Serial.println("Waiting for restart");
  delay(10 * 1000);
  vTaskDelete(restartTaskHandle);



	
}