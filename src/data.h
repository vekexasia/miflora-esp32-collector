#ifndef DATA_H
#define DATA_H
#include <inttypes.h>

namespace garden {
  struct MifloraData {
    char identifier[18];
    uint8_t temperature;
    uint8_t moisture;
    uint16_t light;
    uint16_t conductivity;
    uint8_t battery;
  };

};



#endif