#include <Arduino.h>
#include <Wire.h>
#include "UniversalChuk.h"

#define ADDRESS 0x52

// device type masks
#define N_TYPE_MASK_UPPER  0x0000
#define N_TYPE_MASK_MID    0xA420
#define N_TYPE_MASK_LOWER  0x0000

// technically still the classic controller pro id
#define CC_TYPE_MASK_UPPER 0x0100
#define CC_TYPE_MASK_MID   0xA420
#define CC_TYPE_MASK_LOWER 0x0101

#define G1_TYPE_MASK_UPPER   0x0000
#define G1_TYPE_MASK_MID     0xA420
#define G1_TYPE_MASK_LOWER   0x0103

#define G2_TYPE_MASK_UPPER   0x00A4
#define G2_TYPE_MASK_MID     0x2001
#define G2_TYPE_MASK_LOWER   0x032B

// nunchuck button masks
// N_ prefix is for nunchuk
// from https://wiibrew.org/wiki/Wiimote/Extension_Controllers/Nunchuck

#define N_BC_MASK      0b00000010
#define N_BZ_MASK      0b00000001

// classic controller pro button masks
// CC_ prefix is for classic controller
// from https://wiibrew.org/wiki/Wiimote/Extension_Controllers/Classic_Controller

#define CC_LEFT_MASK      0b00111111
#define CC_RIGHT_X_MASK    0b11000000
#define CC_RIGHT_Y_MASK    0b00011111
#define CC_LEFT_T_UPPER_MASK 0b01100000
#define CC_LEFT_T_LOWER_MASK 0b11100000
#define CC_RIGHT_T_MASK 0b00011111
#define CC_BDR 0b10000000
#define CC_BDD 0b01000000
#define CC_BLT 0b00100000
#define CC_B_MINUS 0b00010000
#define CC_BH 0b00001000
#define CC_B_PLUS 0b00000100
#define CC_BRT 0b00000010
#define CC_BZL 0b10000000
#define CC_BB 0b01000000
#define CC_BY 0b00100000
#define CC_BA 0b00010000
#define CC_BX 0b00001000
#define CC_BZR 0b00000100
#define CC_BDL 0b00000010
#define CC_BDU 0b00000001

// guitar one button masks
// G1_ prefix is for guitar one
// from https://wiibrew.org/wiki/Wiimote/Extension_Controllers/Guitar_Hero_(Wii)_Guitars

#define G1_SX_MASK      0b00111111
#define G1_SY_MASK      0b00111111
#define G1_WB_MASK      0b00011111
#define G1_BD_MASK      0b01000000
#define G1_B_MINUS_MASK 0b00010000
#define G1_B_PLUS_MASK  0b00000100
#define G1_BO_MASK      0b10000000
#define G1_BR_MASK      0b01000000
#define G1_BB_MASK      0b00100000
#define G1_BG_MASK      0b00010000
#define G1_BY_MASK      0b00001000
#define G1_BU_MASK      0b00000001

// guitar two button masks
// G2_ prefix is for guitar two
// from https://wiibrew.org/wiki/Wiimote/Extension_Controllers/Guitar_Hero_(Wii)_Guitars

#define G2_SY_MASK      0b00111111
#define G2_BD_MASK      0b01000000
#define G2_TB_MASK      0b00011111
#define G2_WB_MASK      0b00011111
#define G2_B_MINUS_MASK 0b00010000
#define G2_B_PLUS_MASK  0b00000100
#define G2_BO_MASK      0b10000000
#define G2_BR_MASK      0b01000000
#define G2_BB_MASK      0b00100000
#define G2_BG_MASK      0b00010000
#define G2_BY_MASK      0b00001000
#define G2_BU_MASK      0b00000001

void UniversalChuk::init()
{
    Wire.begin();
    // Wire.setClock(400000);

    UniversalChuk::_sendByte(0x55, 0xF0);
    UniversalChuk::_sendByte(0x00, 0xFB);

    UniversalChuk::deviceType = UniversalChuk::_getDeviceType();

    delay(10);
}

void UniversalChuk::update()
{
    UniversalChuk::_receiveData();

    switch (UniversalChuk::deviceType) {
        case Nunchuk:
            UniversalChuk::analogX =   UniversalChuk::_data[0];
            UniversalChuk::analogY =   UniversalChuk::_data[1];
            UniversalChuk::accelX =   (UniversalChuk::_data[2] << 2) | ((UniversalChuk::_data[5] >> 2) & 3);
            UniversalChuk::accelY =   (UniversalChuk::_data[3] << 2) | ((UniversalChuk::_data[5] >> 4) & 3);
            UniversalChuk::accelZ =   (UniversalChuk::_data[4] << 2) | ((UniversalChuk::_data[5] >> 6) & 3);
            UniversalChuk::zButton = !(UniversalChuk::_data[5] & N_BZ_MASK);
            UniversalChuk::cButton = !(UniversalChuk::_data[5] & N_BC_MASK);
            break;
        case ClassicController:
            UniversalChuk::leftX =           UniversalChuk::_data[0] & CC_LEFT_MASK;
            UniversalChuk::leftY =           UniversalChuk::_data[1] & CC_LEFT_MASK;
            UniversalChuk::rightX =        ((UniversalChuk::_data[0] & CC_RIGHT_X_MASK) >> 3) + ((UniversalChuk::_data[1] & CC_RIGHT_X_MASK) >> 5) + ((UniversalChuk::_data[2] & 0b10000000) >> 7);
            UniversalChuk::rightY =          UniversalChuk::_data[2] & CC_RIGHT_Y_MASK;
            UniversalChuk::leftTrigger =   ((UniversalChuk::_data[2] & CC_LEFT_T_UPPER_MASK) >> 2) + ((UniversalChuk::_data[3] & CC_LEFT_T_LOWER_MASK) >> 5);
            UniversalChuk::rightTrigger =    UniversalChuk::_data[3] & CC_RIGHT_T_MASK;
            UniversalChuk::dpad.right =    !(UniversalChuk::_data[4] & CC_BDR);
            UniversalChuk::dpad.down =     !(UniversalChuk::_data[4] & CC_BDD);
            UniversalChuk::lButton =       !(UniversalChuk::_data[4] & CC_BLT);
            UniversalChuk::minusButton =   !(UniversalChuk::_data[4] & CC_B_MINUS);
            UniversalChuk::homeButton =    !(UniversalChuk::_data[4] & CC_BH);
            UniversalChuk::plusButton =    !(UniversalChuk::_data[4] & CC_B_PLUS);
            UniversalChuk::rButton =       !(UniversalChuk::_data[4] & CC_BRT);
            UniversalChuk::zlButton =      !(UniversalChuk::_data[5] & CC_BZL);
            UniversalChuk::bButton =       !(UniversalChuk::_data[5] & CC_BB);
            UniversalChuk::yButton =       !(UniversalChuk::_data[5] & CC_BY);
            UniversalChuk::aButton =       !(UniversalChuk::_data[5] & CC_BA);
            UniversalChuk::xButton =       !(UniversalChuk::_data[5] & CC_BX);
            UniversalChuk::zrButton =      !(UniversalChuk::_data[5] & CC_BZR);
            UniversalChuk::dpad.left =     !(UniversalChuk::_data[5] & CC_BDL);
            UniversalChuk::dpad.up =       !(UniversalChuk::_data[5] & CC_BDU);

            break;
        case Guitar1:
            UniversalChuk::analogX =              UniversalChuk::_data[0] & G1_SX_MASK;
            UniversalChuk::analogY =              UniversalChuk::_data[1] & G1_SY_MASK;
            UniversalChuk::wammyBar =             UniversalChuk::_data[3];// & G1_WB_MASK;
            UniversalChuk::strumUp =            !(UniversalChuk::_data[5] & G1_BU_MASK);
            UniversalChuk::strumDown =          !(UniversalChuk::_data[4] & G1_BD_MASK);
            UniversalChuk::minusButton =        !(UniversalChuk::_data[4] & G1_B_MINUS_MASK);
            UniversalChuk::plusButton =         !(UniversalChuk::_data[4] & G1_B_PLUS_MASK);
            UniversalChuk::neckButtons.orange = !(UniversalChuk::_data[5] & G1_BO_MASK);
            UniversalChuk::neckButtons.red =    !(UniversalChuk::_data[5] & G1_BR_MASK);
            UniversalChuk::neckButtons.blue =   !(UniversalChuk::_data[5] & G1_BB_MASK);
            UniversalChuk::neckButtons.green =  !(UniversalChuk::_data[5] & G1_BG_MASK);
            UniversalChuk::neckButtons.yellow = !(UniversalChuk::_data[5] & G1_BY_MASK);
            break;
        case Guitar2:
            // Guitar 2 is weird, 1 byte is always missing from the data received
            // UniversalChuk::analogX =              UniversalChuk::_data[0] & G1_SX_MASK;
            UniversalChuk::analogY =              UniversalChuk::_data[0] & G1_SY_MASK;
            UniversalChuk::touchBar =             UniversalChuk::_data[1] & G2_TB_MASK;
            UniversalChuk::wammyBar =             UniversalChuk::_data[2] & G2_WB_MASK;
            UniversalChuk::strumUp =            !(UniversalChuk::_data[4] & G1_BU_MASK);
            UniversalChuk::strumDown =          !(UniversalChuk::_data[3] & G1_BD_MASK);
            UniversalChuk::minusButton =        !(UniversalChuk::_data[3] & G1_B_MINUS_MASK);
            UniversalChuk::plusButton =         !(UniversalChuk::_data[3] & G1_B_PLUS_MASK);
            UniversalChuk::neckButtons.orange = !(UniversalChuk::_data[4] & G1_BO_MASK);
            UniversalChuk::neckButtons.red =    !(UniversalChuk::_data[4] & G1_BR_MASK);
            UniversalChuk::neckButtons.blue =   !(UniversalChuk::_data[4] & G1_BB_MASK);
            UniversalChuk::neckButtons.green =  !(UniversalChuk::_data[4] & G1_BG_MASK);
            UniversalChuk::neckButtons.yellow = !(UniversalChuk::_data[4] & G1_BY_MASK);
            break;
        case Unknown:

            break;
    }
}

DeviceType UniversalChuk::_getDeviceType() {
    UniversalChuk::_sendByte(0x00, 0xFA);

    UniversalChuk::_receiveData();
    
    if (UniversalChuk::_testDeviceFlag(N_TYPE_MASK_UPPER, N_TYPE_MASK_MID, N_TYPE_MASK_LOWER))
        return Nunchuk;
    else if (UniversalChuk::_testDeviceFlag(CC_TYPE_MASK_UPPER, CC_TYPE_MASK_MID, CC_TYPE_MASK_LOWER))
        return ClassicController;
    else if (UniversalChuk::_testDeviceFlag(G1_TYPE_MASK_UPPER, G1_TYPE_MASK_MID, G1_TYPE_MASK_LOWER))
        return Guitar1;
    else if (UniversalChuk::_testDeviceFlag(G2_TYPE_MASK_UPPER, G2_TYPE_MASK_MID, G2_TYPE_MASK_LOWER))
        return Guitar2;
    else
        return Unknown;
}

// only use after calling _receiveData
bool UniversalChuk::_testDeviceFlag(uint16_t upper, uint16_t mid, uint16_t lower) {
    uint16_t deviceUpper =  (UniversalChuk::_data[0] << 8) + (UniversalChuk::_data[1]);
    uint16_t deviceMid =    (UniversalChuk::_data[2] << 8) + (UniversalChuk::_data[3]);
    uint16_t deviceLower =  (UniversalChuk::_data[4] << 8) + (UniversalChuk::_data[5]);

// debug
    // Serial.println("Device ID:");
    // Serial.println(deviceUpper, HEX);
    // Serial.println(deviceMid, HEX);
    // Serial.println(deviceLower, HEX);
    // Serial.println("");

    if ((deviceUpper == upper) && (deviceMid == mid) && (deviceLower == lower))
        return true;
    return false;
}

bool UniversalChuk::isPluggedIn() {
    Wire.beginTransmission(ADDRESS);
    return !Wire.endTransmission();
}

//-----------------------------------------
#define ZERO_PADDING 8

void SPrintZeroPadBin(uint8_t number) {
    char binstr[]="00000000";
    uint8_t i=0;
    uint8_t n=number;

   while(n>0 && i<ZERO_PADDING){
      binstr[ZERO_PADDING-1-i]=n%2+'0';
      ++i;
      n/=2;
   }
	
   Serial.print(binstr);
}
//-----------------------------------------


void UniversalChuk::_receiveData() {
    int count = 0;

    Wire.requestFrom(ADDRESS, 6);

    while(Wire.available())
    {
        UniversalChuk::_data[count] = Wire.read();
        // debug
        // Serial.print(count);
        // Serial.print(": ");
        // SPrintZeroPadBin(UniversalChuk::_data[count]);
        // Serial.println("");

        count++;
    }

    // debug
    // Serial.println("");

    delay(10);
    
    UniversalChuk::_sendByte(0x00, 0x00);
}

void UniversalChuk::_sendByte(byte data, byte location)
{
  Wire.beginTransmission(ADDRESS);

  Wire.write(location);
  Wire.write(data);

  Wire.endTransmission();

  delay(10);
}