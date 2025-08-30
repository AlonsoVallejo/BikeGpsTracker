#include "gpsMgr.h"
#include "board.h"

/*
 * @brief GpsMgr constructor
 * @paramin modem Reference to TinyGsm modem object
 */
GpsMgr::GpsMgr(TinyGsm& modem) : modem(modem) {}

/*
 * @brief Enable GPS power and functionality on the modem.
 */
void GpsMgr::enable() {
    /* Send AT command to enable GPS power */
    modem.sendAT("+CGPIO=0,48,1,1");
    if (modem.waitResponse(10000L) != 1) {
        SerialMon.println("Failed to enable GPS power");
    }
    /* Enable GPS functionality */
    modem.enableGPS();
    SerialMon.println("GPS enabled");
}

/*
 * @brief Disable GPS power and functionality on the modem.
 */
void GpsMgr::disable() {
    /* Send AT command to disable GPS power */
    modem.sendAT("+CGPIO=0,48,1,0");
    if (modem.waitResponse(10000L) != 1) {
        SerialMon.println("Failed to disable GPS power");
    }
    /* Disable GPS functionality */
    modem.disableGPS();
    SerialMon.println("GPS disabled");
}

/*
 * @brief Check if GPS fix is valid by parsing GNSS raw string.
 * @return true if fix status and mode are both "1", false otherwise.
 */
bool GpsMgr::getFix() {
    /* Get raw GNSS string from modem */
    String gnssRaw = modem.getGPSraw();
    SerialMon.println("Raw GNSS data: " + gnssRaw);
    int idx = gnssRaw.indexOf(":");
    String data = (idx < 0) ? gnssRaw : gnssRaw.substring(idx + 1);
    data.trim();
    /* Find first and second comma to extract fix status and mode */
    int firstComma = data.indexOf(",");
    int secondComma = data.indexOf(",", firstComma + 1);
    if (firstComma < 0 || secondComma < 0)  {
        return false;
    }
    String fixStatus = data.substring(0, firstComma);
    String fixMode = data.substring(firstComma + 1, secondComma);
    /* Check if both fix status and mode are "1" (valid fix) */
    if (fixStatus != "1" || fixMode != "1") {
        return false;
    }
    return true;
}

/*
 * @brief Extract latitude and longitude from GNSS raw string.
 * @param out lat Pointer to float to store latitude.
 * @param out lon Pointer to float to store longitude.
 */
void GpsMgr::getLatLon(float* lat, float* lon) {
    /* Get raw GNSS string from modem */
    String gnssRaw = modem.getGPSraw();
    SerialMon.println("Raw GNSS data: " + gnssRaw);
    int idx = gnssRaw.indexOf(":");
    String data = (idx < 0) ? gnssRaw : gnssRaw.substring(idx + 1);
    int fieldCount = 0;
    int lastPos = 0;
    float latitude = 0.0, longitude = 0.0;
    data.trim();
    /* Parse comma-separated fields to extract latitude and longitude */
    for (int i = 0; i < data.length(); ++i) {
        if (data[i] == ',') {
            fieldCount++;
            if (fieldCount == 3) { /* latitude is field 3 */
                int nextComma = data.indexOf(',', i + 1);
                String latStr = data.substring(i + 1, nextComma);
                latitude = latStr.toFloat();
                i = nextComma - 1;
            } else if (fieldCount == 4) { /* longitude is field 4 */
                int nextComma = data.indexOf(',', i + 1);
                String lonStr = data.substring(i + 1, nextComma);
                longitude = lonStr.toFloat();
                break;
            }
        }
    }
    *lat = latitude;
    *lon = longitude;
}

