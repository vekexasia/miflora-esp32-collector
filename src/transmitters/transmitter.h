#ifndef _TRANSMITTER_H
#define _TRANSMITTER_H
#include "../data.h"

namespace garden {
  class Transmitter {
    public:
      Transmitter();
      void setup();
      virtual void spool();
      virtual void broadcast(garden::MifloraData *data);
      void teardown();
  };
}

#endif