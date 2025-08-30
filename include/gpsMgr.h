#pragma once
#include "3rdPartyLibs.h"
#include <Arduino.h>

typedef enum {
    GPS_MODEM_TEST,
    GPS_MODEM_ENABLE,
    GPS_MODEM_DISABLE,
    GPS_MODEM_GET_FIX,
    GPS_MODEM_FIX_ACQUIRED,
    GPS_MODEM_IDLE
} GpsFixType;

class GpsMgr {
public:
    GpsMgr(TinyGsm& modem);
    void enable();
    void disable();
    bool getFix();
    void getLatLon(float* lat, float* lon);
private:
    TinyGsm& modem;
};
