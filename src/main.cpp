#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>
#include "system.h"
#include <TinyGsmClient.h>

/* Create modem and GPS manager instances */
TinyGsm TinyModem(SerialAT);

/*
 * @brief Main FreeRTOS task for GPS acquisition and reporting.
 * @paramin pvParameters Pointer to task parameters (unused).
 */
void gpsTask(void* pvParameters) {
    sysAppData_t* appData = (sysAppData_t*)pvParameters;
    float gps_lat = 0.0, gps_lon = 0.0;
    bool gps_fix_acquired = false;
    static GpsFixType gpsState = GPS_MODEM_TEST;
    while (1) {
        switch (gpsState) {
            case GPS_MODEM_IDLE:
                if(gps_fix_acquired) {
                    SerialMon.println("Entering idle state, will reacquire GPS position after interval");
                    vTaskDelay(pdMS_TO_TICKS(10000));
                    gpsState = GPS_MODEM_ENABLE;
                } else {
                    SerialMon.println("GPS fix not acquired, staying in idle state");
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            break;
            case GPS_MODEM_TEST:
                if (!appData->modemMgr->test()) {
                    SerialMon.println("Modem test failed: Restarting modem");
                    appData->modemMgr->restart();
                } else {
                    gpsState = GPS_MODEM_ENABLE;
                }
                vTaskDelay(pdMS_TO_TICKS(1000));
            break;
            case GPS_MODEM_ENABLE:
                appData->modemMgr->GpsEnable();
                SerialMon.println("Start GPS positioning!");
                if(gps_fix_acquired) {
                    gpsState = GPS_MODEM_FIX_ACQUIRED;
                } else {
                    gpsState = GPS_MODEM_GET_FIX;
                }
                vTaskDelay(pdMS_TO_TICKS(1000));
            break;
            case GPS_MODEM_GET_FIX:
                if (appData->modemMgr->GpsGetFix()) {
                    SerialMon.println("GPS fix acquired!");
                    gps_fix_acquired = true;
                    gpsState = GPS_MODEM_FIX_ACQUIRED;
                    vTaskDelay(pdMS_TO_TICKS(1000));
                } else {
                    SerialMon.println("Waiting for GPS fix...");
                    TOGGLE_LED();
                    vTaskDelay(pdMS_TO_TICKS(2000));
                }
            break;
            case GPS_MODEM_FIX_ACQUIRED:
                appData->modemMgr->GpsGetLatLon(&appData->gpsData->latitude, &appData->gpsData->longitude);
                SerialMon.println("Latitude: " + String(appData->gpsData->latitude, 6) + ", Longitude: " + String(appData->gpsData->longitude, 6));
                gpsState = GPS_MODEM_DISABLE;
                vTaskDelay(pdMS_TO_TICKS(100));
            break;
            case GPS_MODEM_DISABLE:
                SerialMon.println("Disabling GPS...");
                appData->modemMgr->GpsDisable();
                gpsState = GPS_MODEM_IDLE;
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
            default:
                gpsState = GPS_MODEM_TEST;
                gps_fix_acquired = false;
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
        }
    }
}

void cellularTask(void* pvParameters) {
    sysAppData_t* appData = (sysAppData_t*)pvParameters;
    for (;;) {
        // Cellular task logic here
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*
 * @brief Arduino setup function. Initializes hardware and starts GPS task.
 */
void setup() {
    SerialMon.begin(115200);
    SerialAT.begin(MODEM_UART_BAUD, SERIAL_8N1, MODEM_PIN_RX, MODEM_PIN_TX);

    pinMode(USER_BLUE_LED_PIN, OUTPUT);
    TURN_ON_LED();

    static sysGpsData_t sysGpsData = {
        0.0, 
        0.0, 
        false, 
        GPS_MODEM_IDLE
    };

    static sysCellData_t sysCellData = {
        "+523121084676",
        "Bike GPS Tracker position:", 
        "", 
        ""
    };

    /* Create SIM7070G manager instances */
    static ModemMgr sim7070g(TinyModem, SerialMon, SerialAT, MODEM_PWR_PIN, MODEM_PIN_DTR);

    sim7070g.init();
    sim7070g.powerOn();
    sim7070g.awake(); 
    SerialMon.println("Modem initialized.");

    static sysAppData_t sysAppData = {
        &sim7070g,
        &sysGpsData,
        &sysCellData
    };

    xTaskCreatePinnedToCore(gpsTask, "GpsTask", 4096, &sysAppData, 1, NULL, 1);
    xTaskCreatePinnedToCore(cellularTask, "CellularTask", 4096, &sysAppData, 1, NULL, 1);
}

/*
 * @brief Arduino main loop. Not used, as logic is handled by FreeRTOS tasks.
 */
void loop() {
}