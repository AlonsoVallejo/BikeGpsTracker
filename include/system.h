#include "modemMgr.h"
#include "gpsMgr.h"
#include "cellularMgr.h"

#define USER_BLUE_LED_PIN 12
#define TURN_OFF_LED() digitalWrite(USER_BLUE_LED_PIN, HIGH)
#define TURN_ON_LED()  digitalWrite(USER_BLUE_LED_PIN, LOW)
#define TOGGLE_LED()   digitalWrite(USER_BLUE_LED_PIN, !digitalRead(USER_BLUE_LED_PIN))

struct sysAppData_t {
    ModemMgr* modemMgr;
    GpsMgr* gpsMgr;
    sysGpsData_t* gpsData;
    sysCellData_t* cellData;
};
