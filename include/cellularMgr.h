#pragma once
#include <Arduino.h>

/* SMS text sent to target phone number: "GPS_MAP_URL" + "," + "latitude","longitude" */
/* i.e.: "http://www.google.com/maps/place/20.558853,-103.428903" */
#define GPS_MAP_URL "http://www.google.com/maps/place/"

struct sysCellData_t {
    String target_number;
    String msg_txt_sms;
    String msg_lat_buf;
    String msg_lon_buf;
};
