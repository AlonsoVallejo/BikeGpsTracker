#pragma once
#include "3rdPartyLibs.h"
#include <Arduino.h>

class ModemMgr {
public:
    ModemMgr(TinyGsm& modem, HardwareSerial& serialMon, HardwareSerial& serialAT, int pwrPin, int dtrPin);
    void powerOn();
    void powerOff();
    void restart();
    bool test();
    void awake();
private:
    TinyGsm& modem;
    HardwareSerial& serialMon;
    HardwareSerial& serialAT;
    int pwrPin;
    int dtrPin;
};
