#pragma once
#include <Arduino.h>
#include "modemMgr.h"

typedef enum {
    GPS_MODEM_TEST,
    GPS_MODEM_ENABLE,
    GPS_MODEM_DISABLE,
    GPS_MODEM_GET_FIX,
    GPS_MODEM_FIX_ACQUIRED,
    GPS_MODEM_IDLE
} GpsFixType;

struct sysGpsData_t {
    float latitude;
    float longitude;
    bool gps_fix_acquired;
    GpsFixType gpsFixStatus;
};

class GpsMgr : public ModemMgr {
public:
    GpsMgr(TinyGsm& modem, HardwareSerial& serialMon, HardwareSerial& serialAT, int pwrPin, int dtrPin);
    void enable();
    void disable();
    bool getFix();
    void getLatLon(float* lat, float* lon);
};
