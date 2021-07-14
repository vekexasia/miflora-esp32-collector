#ifndef COMFOWIFI_H
#define COMFOWIFI_H
namespace garden {
  class WiFi {
    public:
      void setup();
      void teardown();
    private:
      bool tryConnect();
  };
}
#endif
