#include "../secrets.h"
#include <WiFi.h>
#include "wifi.h"


namespace garden {
  void WiFi::setup() {
    ::WiFi.setTxPower(WIFI_POWER_19_5dBm);
    this->tryConnect();
  }

  bool WiFi::isConnected() {
    return ::WiFi.status() == WL_CONNECTED;
  }

  bool WiFi::tryConnect() {
    if (::WiFi.status() != WL_CONNECTED) {
      Serial.println("Trying to connect to WiFi");
      ::WiFi.disconnect();
      ::WiFi.begin(WIFI_SSID, WIFI_PASS);
      uint8_t maxTry = 8;
      while (::WiFi.status() != WL_CONNECTED && maxTry >= 0) {
        delay(1000);
        Serial.print(".");
        maxTry--;
      }
    }

    return ::WiFi.status() == WL_CONNECTED;
  }

  void WiFi::teardown() {

    // ::WiFi.disconnect();
  }
} 