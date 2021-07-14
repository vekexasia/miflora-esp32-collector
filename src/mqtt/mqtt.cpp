#include <WiFi.h>
#include <map>
#include "../secrets.h"
#include <mqtt_client.h>
#include "mqtt.h"

bool mqttConnected = false;
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    static const char *TAG = "mqtt_event";

    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        mqttConnected = true;
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqttConnected = false;
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        printf("TOPIC=%d , %.*s\r\n", event->topic_len, event->topic_len, event->topic);
        printf("DATA=%d , %.*s\r\n", event->data_len, event->data_len, event->data);
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }

    return ESP_OK;
}
const esp_mqtt_client_config_t mqtt_cfg = {
    .event_handle = mqtt_event_handler,
    .uri = MQTT_URL,
    // .port = 1883,
    // .keepalive = 1,
    // .transport = MQTT_TRANSPORT_OVER_TCP,
};
namespace garden {

  esp_mqtt_client_handle_t mqttClient;
  MQTT::MQTT() {
    mqttClient = esp_mqtt_client_init(&mqtt_cfg);
  }


  void MQTT::setup() {
    esp_mqtt_client_start(mqttClient);
    Serial.println("Starting mqtt");
  }

  bool MQTT::isConnected() {
    // Serial.println("MqttConnected");
    // Serial.println(mqttConnected);
    // Serial.println(::WiFi.status() == WL_CONNECTED);
    // Serial.println(mqttClient != NULL );
    return ::WiFi.status() == WL_CONNECTED && mqttClient != NULL && mqttConnected;
  }

  bool MQTT::tryConnectOnce() {
    if (::WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected.");
      return false;
    }
    Serial.println("Attempting MQTT connection...");
    // return false;
    // Create a random client ID
    // Attempt to connect
    if (ESP_OK == esp_mqtt_client_reconnect(mqttClient) ) {
      return true;
    } else {
      return false;
    }
  }

  void MQTT::loop() {
  }

  void MQTT::writeToTopic(char * topic, char * payload, bool sticky) {
    this->ensureConnected();
    Serial.print("Writing");
    Serial.print(topic);
    Serial.print("=");
    Serial.println(payload);
    
    esp_mqtt_client_publish(
      mqttClient,
      topic,
      payload,
      0,
      1,
      1
    );
    // this->client.publish(topic, payload, sticky);
    // wifiClient.flush();
  }

// PRIVATE STUFF

  void MQTT::ensureConnected() {
    // Loop until we're reconnected
    // while (!this->client.connected()) {
    //   if (!this->tryConnectOnce()) {
    //     // wait 5s before retrying
    //     delay(5000);
    //   }
    // }
  }

  void MQTT::teardown() {
    esp_mqtt_client_stop(mqttClient);
    esp_mqtt_client_destroy(mqttClient);
    // client.disconnect();
  }

} // namespace garden