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

/* SMS text sent to target phone number: "GPS_MAP_URL" + "," + "latitude","longitude" */
/* i.e.: "http://www.google.com/maps/place/20.558853,-103.428903" */
#define GPS_MAP_URL "http://www.google.com/maps/place/"

struct sysCellData_t {
    String target_number;
    String msg_txt_sms;
    String msg_lat_buf;
    String msg_lon_buf;
};

#define USER_BLUE_LED_PIN 12
#define TURN_OFF_LED() digitalWrite(USER_BLUE_LED_PIN, HIGH)
#define TURN_ON_LED()  digitalWrite(USER_BLUE_LED_PIN, LOW)
#define TOGGLE_LED()   digitalWrite(USER_BLUE_LED_PIN, !digitalRead(USER_BLUE_LED_PIN))

struct sysAppData_t {
    ModemMgr* modemMgr;
    sysGpsData_t* gpsData;
    sysCellData_t* cellData;
};
