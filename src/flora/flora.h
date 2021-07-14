#ifndef FLORA_H
#define FLORA_H
#include <BLEDevice.h>
#include "../transmitters/transmitter.h"
namespace garden {
  class Flora {
    public:
      Flora(garden::Transmitter *_my433);
      void setup();
      void loop();
      void teardown();
    private:
      garden::Transmitter *transmitter;

      BLEClient* getFloraClient(BLEAddress floraAddress);
      BLERemoteService* getFloraService(BLEClient* floraClient);
      bool forceFloraServiceDataMode(BLERemoteService* floraService);
      bool readFloraDataCharacteristic(BLERemoteService* floraService);
      bool readFloraBatteryCharacteristic(BLERemoteService* floraService);
      bool processFloraService(BLERemoteService* floraService, char* deviceMacAddress, bool readBattery);
      bool processFloraDevice(BLEAddress floraAddress, char* deviceMacAddress, bool getBattery, int tryCount);
  };
}

#endif