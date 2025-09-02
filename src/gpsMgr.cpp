#include "gpsMgr.h"

/*
 * @brief GpsMgr constructor
 * @paramin modem Reference to TinyGsm modem object
 * @paramin serialMon Reference to Serial monitor
 * @paramin serialAT Reference to Serial AT port
 * @paramin pwrPin Modem power control pin
 * @paramin dtrPin Modem DTR (sleep/wake) pin
 */
GpsMgr::GpsMgr(TinyGsm& modem, HardwareSerial& serialMon, HardwareSerial& serialAT, int pwrPin, int dtrPin)
    : ModemMgr(modem, serialMon, serialAT, pwrPin, dtrPin) {}

/*
 * @brief Enable GPS power and functionality on the modem.
 */
void GpsMgr::enable() {
    modem.sendAT("+CGPIO=0,48,1,1");
    if (modem.waitResponse(10000L) != 1) {
        serialMon.println("Failed to enable GPS power");
    }
    modem.enableGPS();
    serialMon.println("GPS enabled");
}

/*
 * @brief Disable GPS power and functionality on the modem.
 */
void GpsMgr::disable() {
    modem.sendAT("+CGPIO=0,48,1,0");
    if (modem.waitResponse(10000L) != 1) {
        serialMon.println("Failed to disable GPS power");
    }
    modem.disableGPS();
    serialMon.println("GPS disabled");
}

/*
 * @brief Check if GPS fix is valid by parsing GNSS raw string.
 * @return true if fix status and mode are both "1", false otherwise.
 */
bool GpsMgr::getFix() {
    String gnssRaw = modem.getGPSraw();
    serialMon.println("Raw GNSS data: " + gnssRaw);
    int idx = gnssRaw.indexOf(":");
    String data = (idx < 0) ? gnssRaw : gnssRaw.substring(idx + 1);
    data.trim();
    int firstComma = data.indexOf(",");
    int secondComma = data.indexOf(",", firstComma + 1);
    if (firstComma < 0 || secondComma < 0)  {
        return false;
    }
    String fixStatus = data.substring(0, firstComma);
    String fixMode = data.substring(firstComma + 1, secondComma);
    if (fixStatus != "1" || fixMode != "1") {
        return false;
    }
    return true;
}

/*
 * @brief Extract latitude and longitude from GNSS raw string.
 * @paramout lat Pointer to float to store latitude.
 * @paramout lon Pointer to float to store longitude.
 */
void GpsMgr::getLatLon(float* lat, float* lon) {
    String gnssRaw = modem.getGPSraw();
    serialMon.println("Raw GNSS data: " + gnssRaw);
    int idx = gnssRaw.indexOf(":");
    String data = (idx < 0) ? gnssRaw : gnssRaw.substring(idx + 1);
    int fieldCount = 0;
    int lastPos = 0;
    float latitude = 0.0, longitude = 0.0;
    data.trim();
    for (int i = 0; i < data.length(); ++i) {
        if (data[i] == ',') {
            fieldCount++;
            if (fieldCount == 3) { // latitude is field 3
                int nextComma = data.indexOf(',', i + 1);
                String latStr = data.substring(i + 1, nextComma);
                latitude = latStr.toFloat();
                i = nextComma - 1;
            } else if (fieldCount == 4) { // longitude is field 4
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

