#include <PubSubClient.h>
#include <WiFi.h>
#include <map>
#include "../secrets.h"
#include "./_433.h"
#include "../data.h"
#include <RFTransmitter.h>


namespace garden {
  char _433Buf[64];
  RFTransmitter transmitter(26, 1);

  void _433::broadcast(garden::MifloraData *data) {
    Serial.println("Broadcasting");
    sprintf(_433Buf, "%s,%d,%d,%d,%d,%d",data->identifier, data->temperature, data->moisture, data->conductivity, data->light, data->battery);
    Serial.println(_433Buf);
    transmitter.send((byte *)_433Buf, strlen(_433Buf)+1);
  }


}