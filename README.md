# MiFlora Esp32 Collector

This software let you use ESP32 + several miflora like garden sensor to collect your lawn data.

# secrets.h

In order to compile and deploy this piece of software you need to create a `secrets.h` file which needs the following variables

```
// The mqtt prefix where you'd like to push data.
#define MQTT_PREFIX "garden"
// The MQTT connection url
#define MQTT_URL "mqtt://user:pass@mqtt_ip:mqtt_port"
// The Wifi AP Name
#define WIFI_SSID "MySuperSecureWiFi"
// The Wifi Password
#define WIFI_PASS "password12345679:)"
// Array of mac address to look for.
#define FLORA_DEVICES_ALL { \
  "80:EA:CA:89:14:CF", \
  "80:EA:CA:89:22:06", \
  "C4:7C:8D:6B:53:00", \
  "C4:7C:8D:6B:54:92", \
}
```

# This project on Battery

This codebase could work running your ESP32 on battery (preferably LFP 18650) but you'll have to do more code changes expecially to turn on deep-sleep and have less frequent wake-ups.

# Known bugs

Randomly this piece of code seems to stop working and sending data. Due to the random nature of the bug and the little time I got, I couldn't really debug why that happens.

# Other integrations

Initially the project was conceived to use 433Mhz transceivers instead of WiFi+MQTT so there is still some codebase related to that.
