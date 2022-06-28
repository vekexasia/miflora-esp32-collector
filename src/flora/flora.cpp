#include "flora.h"
#include <Arduino.h>
#include "../secrets.h"
#include <BLEDevice.h>
#include "../transmitters/transmitter.h"

char *FLORA_DEVICES[] = FLORA_DEVICES_ALL;

// device count
int FLORA_DEVICES_COUNT = sizeof FLORA_DEVICES / sizeof FLORA_DEVICES[0];

// the remote service we wish to connect to
static BLEUUID serviceUUID("00001204-0000-1000-8000-00805f9b34fb");

// the characteristic of the remote service we are interested in
static BLEUUID uuid_version_battery("00001a02-0000-1000-8000-00805f9b34fb");
static BLEUUID uuid_sensor_data("00001a01-0000-1000-8000-00805f9b34fb");
static BLEUUID uuid_write_mode("00001a00-0000-1000-8000-00805f9b34fb");
garden::MifloraData mifloraData;
BLEScan* pBLEScan;

namespace garden {

  
Flora::Flora(garden::Transmitter *transmitter) {
  this->transmitter = transmitter;
}

void Flora::setup() {
  BLEDevice::init("ESP32");
  // BLEDevice::setPower(ESP_PWR_LVL_P9);
  //  // process devices
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  esp_bt_controller_enable(ESP_BT_MODE_BLE);
	esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);
}

void Flora::loop() {
  BLEScanResults foundDevices = pBLEScan->start(5, false);

Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  for (int j=0; j<foundDevices.getCount(); j++) {
    BLEAddress discoveredAddr = foundDevices.getDevice(j).getAddress();
    for (int i=0; i<FLORA_DEVICES_COUNT; i++) {
        char* deviceMacAddress = FLORA_DEVICES[i];
        BLEAddress floraAddress(deviceMacAddress);
        if (floraAddress.equals(discoveredAddr)) {
            Serial.print(deviceMacAddress); Serial.println("Found");
            if (this->processFloraDevice(floraAddress, deviceMacAddress, esp_random() % 3 == 0, 1)) {
                Serial.println("ALL GOOD");
                memcpy(mifloraData.identifier, deviceMacAddress, 18);
                this->transmitter->broadcast(&mifloraData);
                Serial.println("PD");    
            }
            delay(1000);
            break;
        }
        
    }
  }
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  
}

void Flora::teardown() {
  BLEDevice::deinit();
}

BLEClient *Flora::getFloraClient(BLEAddress floraAddress) {
    BLEClient *floraClient = BLEDevice::createClient();
    // esp_bd_addr_t addr;
    // memcpy(addr,floraAddress.getNative(),6);
    // esp_ble_gap_set_prefer_conn_params(addr, 0x06,0x40, 0, 0x03e8);
    if (!floraClient->connect(floraAddress)) {
        Serial.println("- Connection failed, skipping");
        return nullptr;
    }

    Serial.println("- Connection successful");
    return floraClient;
}

BLERemoteService *Flora::getFloraService(BLEClient *floraClient) {
    BLERemoteService *floraService = nullptr;

    try {
        floraService = floraClient->getService(serviceUUID);
    } catch (...) {
        // something went wrong
    }
    if (floraService == nullptr) {
        Serial.println("- Failed to find data service");
    } else {
        Serial.println("- Found data service");
    }

    return floraService;
}

bool Flora::forceFloraServiceDataMode(BLERemoteService *floraService) {
    BLERemoteCharacteristic *floraCharacteristic;

    // get device mode characteristic, needs to be changed to read data
    Serial.println("- Force device in data mode");
    floraCharacteristic = nullptr;
    try {
        floraCharacteristic = floraService->getCharacteristic(uuid_write_mode);
    } catch (...) {
        // something went wrong
    }
    if (floraCharacteristic == nullptr) {
        Serial.println("-- Failed, skipping device");
        return false;
    }

    // write the magic data
    uint8_t buf[2] = {0xA0, 0x1F};
    floraCharacteristic->writeValue(buf, 2, true);

    delay(500);
    return true;
}

bool Flora::readFloraDataCharacteristic(BLERemoteService *floraService) {
    BLERemoteCharacteristic *floraCharacteristic = nullptr;

    // get the main device data characteristic
    Serial.println("- Access characteristic from device");
    try {
        floraCharacteristic = floraService->getCharacteristic(uuid_sensor_data);
    } catch (...) {
        // something went wrong
    }
    if (floraCharacteristic == nullptr) {
        Serial.println("-- Failed, skipping device");
        return false;
    }

    // read characteristic value
    Serial.println("- Read value from characteristic");
    std::string value;
    try {
        value = floraCharacteristic->readValue();
    } catch (...) {
        // something went wrong
        Serial.println("-- Failed, skipping device");
        return false;
    }
    const char *val = value.c_str();

    Serial.print("Hex: ");
    for (int i = 0; i < 16; i++) {
        Serial.print((int)val[i], HEX);
        Serial.print(" ");
    }
    Serial.println(" ");

    int16_t *temp_raw = (int16_t *)val;
    float temperature = (*temp_raw) / ((float)10.0);
    Serial.print("-- Temperature: ");
    Serial.println(temperature);

    int moisture = val[7];
    Serial.print("-- Moisture: ");
    Serial.println(moisture);

    int light = val[3] + val[4] * 256;
    Serial.print("-- Light: ");
    Serial.println(light);

    int conductivity = val[8] + val[9] * 256;
    Serial.print("-- Conductivity: ");
    Serial.println(conductivity);

    if (temperature > 200) {
        Serial.println("-- Unreasonable values received, skip publish");
        return false;
    }


    mifloraData.conductivity = conductivity;
    mifloraData.light = light;
    mifloraData.moisture = moisture;
    mifloraData.temperature = temperature;


    return true;
}

bool Flora::readFloraBatteryCharacteristic(BLERemoteService *floraService) {
    BLERemoteCharacteristic *floraCharacteristic = nullptr;

    // get the device battery characteristic
    Serial.println("- Access battery characteristic from device");
    try {
        floraCharacteristic = floraService->getCharacteristic(uuid_version_battery);
    } catch (...) {
        // something went wrong
    }
    if (floraCharacteristic == nullptr) {
        Serial.println("-- Failed, skipping battery level");
        return false;
    }

    // read characteristic value
    Serial.println("- Read value from characteristic");
    std::string value;
    try {
        value = floraCharacteristic->readValue();
    } catch (...) {
        // something went wrong
        Serial.println("-- Failed, skipping battery level");
        return false;
    }
    const char *val2 = value.c_str();
    int battery = val2[0];

    mifloraData.battery = battery;

    return true;
}

bool Flora::processFloraService(BLERemoteService *floraService, char *deviceMacAddress, bool readBattery) {
    // set device in data mode
    if (!forceFloraServiceDataMode(floraService)) {
        return false;
    }
    bool dataSuccess = this->readFloraDataCharacteristic(floraService);

    bool batterySuccess = true;
    if (readBattery) {
        batterySuccess = this->readFloraBatteryCharacteristic(floraService);
    } else {
      mifloraData.battery = 255;
    }
    return dataSuccess && batterySuccess;
}

bool Flora::processFloraDevice(BLEAddress floraAddress, char *deviceMacAddress, bool getBattery, int tryCount) {
    Serial.print("Processing Flora device at ");
    Serial.print(floraAddress.toString().c_str());
    Serial.print(" (try ");
    Serial.print(tryCount);
    Serial.println(")");

    // connect to flora ble server
    BLEClient *floraClient = this->getFloraClient(floraAddress);
    if (floraClient == nullptr) {
        return false;
    }

    // connect data service
    BLERemoteService *floraService = this->getFloraService(floraClient);
    if (floraService == nullptr) {
        floraClient->disconnect();
        return false;
    }

    // process devices data
    bool success = this->processFloraService(floraService, deviceMacAddress, getBattery);

    // disconnect from device
    floraClient->disconnect();

    return success;
}

} // namespace garden