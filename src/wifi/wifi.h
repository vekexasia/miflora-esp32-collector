#ifndef COMFOWIFI_H
#define COMFOWIFI_H
namespace garden {
  class WiFi {
    public:
      void setup();
      void teardown();
      bool isConnected();
    private:
      bool tryConnect();
  };
}
#endif
