#ifndef _MQTTTRANSMITTER_H
#define _MQTTTRANSMITTER_H
#include "../data.h"
#include <map>

#include "./transmitter.h"

namespace garden {
  class MQTTTransmitter: public garden::Transmitter {
    public:
      MQTTTransmitter();
      void broadcast(garden::MifloraData *data) override;
      void spool() override;
  };
}

#endif