#ifndef MQTT_H
#define MQTT_H

#include <inttypes.h>
#include <map>

namespace garden {
  class MQTT {
    public:
      MQTT();
      void setup();
      void loop();
      void writeToTopic(char *topic, char* payload, bool sticky);
      void teardown();
      bool isConnected();
      bool tryConnectOnce();
    private:
      void ensureConnected();
  };
}

#endif