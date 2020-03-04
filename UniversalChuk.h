#ifndef UniversalChuk_H
#define UniversalChuk_H

#include <Arduino.h>

struct GuitarButtonGroup {
    bool green;
    bool red;
    bool yellow;
    bool blue;
    bool orange;
};

struct DpadGroup {
    bool up;
    bool down;
    bool left;
    bool right;
};

enum DeviceType {
    Nunchuk,
    ClassicController,
    Guitar1,
    Guitar2,
    Unknown
};

class UniversalChuk
{
  public:
  // type: nunchuk
    int analogX;
    int analogY;
    int accelX;
    int accelY;
    int accelZ;
    bool cButton;
    bool zButton;
  // type: guitar
    GuitarButtonGroup neckButtons;
    int touchBar;
    int wammyBar;
    bool strumUp;
    bool strumDown;
    bool minusButton;
    bool plusButton;
  // type: classic controller
    bool aButton;
    bool bButton;
    bool xButton;
    bool yButton;
    bool homeButton;
    DpadGroup dpad;
    int leftX;
    int leftY;
    int rightX;
    int rightY;
    int leftTrigger;
    int rightTrigger;
    bool rButton;
    bool zrButton;
    bool lButton;
    bool zlButton;

    DeviceType deviceType;

    void init();
    void update();
    bool isPluggedIn();

  private:
    void _sendByte(byte data, byte location);
    void _receiveData();
    byte _data[6];
    DeviceType _getDeviceType();
    bool _testDeviceFlag(uint16_t upper, uint16_t mid, uint16_t lower);
};

#endif
