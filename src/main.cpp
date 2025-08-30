#include <Arduino.h>
#include "board.h"
#include "3rdPartyLibs.h"
#include "modemMgr.h"
#include "gpsMgr.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define DUMP_AT_COMMANDS
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

ModemMgr modemMgr(modem, SerialMon, SerialAT, MODEM_PWR_PIN, MODEM_PIN_DTR);
GpsMgr gpsMgr(modem);


/*
 * @brief Main FreeRTOS task for GPS acquisition and reporting.
 * @paramin pvParameters Pointer to task parameters (unused).
 */
void gpsTask(void* pvParameters) {
    float gps_lat = 0.0, gps_lon = 0.0;
    bool gps_fix_acquired = false;
    static GpsFixType gpsState = GPS_MODEM_TEST;
    while (1) {
        switch (gpsState) {
            case GPS_MODEM_IDLE:
                /* If a fix was acquired, wait before reacquiring */
                if(gps_fix_acquired) {
                    SerialMon.println("Entering idle state, will reacquire GPS fix after interval");
                    /* Wait for reacquire interval, then start new fix cycle */
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    gps_fix_acquired = false;
                    gpsState = GPS_MODEM_TEST;
                } else {
                    SerialMon.println("GPS fix not acquired, staying in idle state");
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            break;
            case GPS_MODEM_TEST:
                /* Test modem communication before enabling GPS */
                if (!modemMgr.test()) {
                    SerialMon.println("Modem test failed: Restarting modem");
                    modemMgr.restart();
                } else {
                    gpsState = GPS_MODEM_ENABLE;
                }
                vTaskDelay(pdMS_TO_TICKS(1000));
            break;
            case GPS_MODEM_ENABLE:
                /* Enable GPS hardware and start positioning */
                gpsMgr.enable();
                SerialMon.println("Start GPS positioning!");
                gpsState = GPS_MODEM_GET_FIX;
                vTaskDelay(pdMS_TO_TICKS(1000));
            break;
            case GPS_MODEM_GET_FIX:
                /* Check for GPS fix */
                if (gpsMgr.getFix()) {
                    SerialMon.println("GPS fix acquired!");
                    gpsState = GPS_MODEM_FIX_ACQUIRED;
                    vTaskDelay(pdMS_TO_TICKS(1000));
                } else {
                    SerialMon.println("Waiting for GPS fix...");
                    TOGGLE_LED();
                    vTaskDelay(pdMS_TO_TICKS(2000));
                }
            break;
            case GPS_MODEM_FIX_ACQUIRED:
                /* After reporting, disable GPS and go to idle for reacquire interval */
                gpsMgr.getLatLon(&gps_lat, &gps_lon);
                SerialMon.println("Latitude: " + String(gps_lat, 6) + ", Longitude: " + String(gps_lon, 6));
                gps_fix_acquired = true;
                gpsState = GPS_MODEM_DISABLE;
                vTaskDelay(pdMS_TO_TICKS(100));
            break;
            case GPS_MODEM_DISABLE:
                /* Disable GPS hardware to save power */
                SerialMon.println("Disabling GPS...");
                gpsMgr.disable();
                gpsState = GPS_MODEM_IDLE;
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
            default:
                gpsState = GPS_MODEM_TEST;
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
        }
    }
}

/*
 * @brief Arduino setup function. Initializes hardware and starts GPS task.
 */
void setup() {
    SerialMon.begin(115200);
    /* Configure hardware pins */
    pinMode(USER_BLUE_LED_PIN, OUTPUT);
    pinMode(MODEM_PWR_PIN, OUTPUT);
    pinMode(MODEM_PIN_DTR, OUTPUT);
    TURN_ON_LED();
    /* Power on and wake modem */
    modemMgr.powerOn();
    modemMgr.awake(); /* Pull down DTR to ensure the modem is not in sleep state */
    /* Initialize modem serial port */
    SerialAT.begin(MODEM_UART_BAUD, SERIAL_8N1, MODEM_PIN_RX, MODEM_PIN_TX);
    SerialMon.println("Initializing modem...");
    /* Start GPS FreeRTOS task */
    xTaskCreate(gpsTask, "GpsTask", 4096, NULL, 1, NULL);
}

/*
 * @brief Arduino main loop. Not used, as logic is handled by FreeRTOS tasks.
 */
void loop() {
}