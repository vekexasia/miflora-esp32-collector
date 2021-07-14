#ifndef _433_H
#define _433_H
#include "../data.h"

#include "./transmitter.h"

namespace garden {
  class _433: public garden::Transmitter {
    public:
      void broadcast(garden::MifloraData *data) override;
  };
}

#endif