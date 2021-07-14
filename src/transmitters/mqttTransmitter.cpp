#include <PubSubClient.h>
#include <WiFi.h>
#include <map>
#include "../secrets.h"
#include "./mqttTransmitter.h"
#include "../data.h"
#include "../mqtt/mqtt.h"
#include <Preferences.h>

extern garden::MQTT *mqtt;
extern Preferences prefs;
extern char *FLORA_DEVICES[];
extern int FLORA_DEVICES_COUNT;
char key[13];
namespace garden {
  MQTTTransmitter::MQTTTransmitter() {
  }
  void MQTTTransmitter::broadcast(garden::MifloraData *data) {
        // Serial.println("aDC");
    Serial.println(data->identifier);

    key[0] = data->identifier[0];
    key[1] = data->identifier[1];
    key[2] = data->identifier[3];
    key[3] = data->identifier[4];
    key[4] = data->identifier[6];
    key[5] = data->identifier[7];
    key[6] = data->identifier[9];
    key[7] = data->identifier[10];
    key[8] = data->identifier[12];
    key[9] = data->identifier[13];
    key[10] = data->identifier[15];
    key[11] = data->identifier[16];
    key[12] = '\0';
    prefs.putBytes(key, data, sizeof(*data));
  }

  void MQTTTransmitter::spool() {
    if (!mqtt->isConnected()) {
      mqtt->tryConnectOnce();
    }
    if (!mqtt->isConnected()) {
      Serial.println("MQTT NOT CONNECTED :(");
      return;
    }
    mqtt->loop();
    char topic[64];
    char payload[64];

    for (uint8_t i=0; i<FLORA_DEVICES_COUNT; i++) {
      key[0] = FLORA_DEVICES[i][0];
      key[1] = FLORA_DEVICES[i][1];
      key[2] = FLORA_DEVICES[i][3];
      key[3] = FLORA_DEVICES[i][4];
      key[4] = FLORA_DEVICES[i][6];
      key[5] = FLORA_DEVICES[i][7];
      key[6] = FLORA_DEVICES[i][9];
      key[7] = FLORA_DEVICES[i][10];
      key[8] = FLORA_DEVICES[i][12];
      key[9] = FLORA_DEVICES[i][13];
      key[10] = FLORA_DEVICES[i][15];
      key[11] = FLORA_DEVICES[i][16];
      key[12] = '\0';
      
    Serial.println(key);
    size_t length = prefs.getBytesLength(key);
    if (length >0 ) {
      Serial.print("Data for: ");
      Serial.println(FLORA_DEVICES[i]);
      char buf[length];
      prefs.getBytes(key, buf, length);
      MifloraData *data = (MifloraData*) buf;
      sprintf(topic, "%s/%s/temperature", MQTT_PREFIX, data->identifier);
      sprintf(payload, "%d", data->temperature);
      mqtt->writeToTopic(topic, payload, true);
      sprintf(topic, "%s/%s/moisture", MQTT_PREFIX, data->identifier);
      sprintf(payload, "%d", data->moisture);
      mqtt->writeToTopic(topic, payload, true);
      sprintf(topic, "%s/%s/conductivity", MQTT_PREFIX, data->identifier);
      sprintf(payload, "%d", data->conductivity);
      mqtt->writeToTopic(topic, payload, true);
      sprintf(topic, "%s/%s/light", MQTT_PREFIX, data->identifier);
      sprintf(payload, "%d", data->light);
      mqtt->writeToTopic(topic, payload, true);
      if (data->battery != 255) {
        sprintf(topic, "%s/%s/battery", MQTT_PREFIX, data->identifier);
        sprintf(payload, "%d", data->battery);
        mqtt->writeToTopic(topic, payload, true);
      }
      prefs.remove(key);
      }
    }
    Serial.println("Finished mqtttransmitter");
  }

}