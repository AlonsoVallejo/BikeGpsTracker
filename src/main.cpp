#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>
#include "system.h"
#include <TinyGsmClient.h>

/* Create modem and GPS manager instances */
TinyGsm TinyModem(SerialAT);

/* Mutex for modem access */
SemaphoreHandle_t modemMutex;

/*
 * @brief Main FreeRTOS task for GPS acquisition and reporting.
 * @paramin pvParameters Pointer to task parameters (unused).
 */
void gpsTask(void* pvParameters) {
    sysAppData_t* appData = (sysAppData_t*)pvParameters;
    static GpsFixType gpsState = GPS_MODEM_TEST;
    while (1) {
        // Before using GNSS functions
        if (xSemaphoreTake(modemMutex, portMAX_DELAY) == pdTRUE) {
            switch (gpsState) {
                case GPS_MODEM_IDLE:
                    if(appData->gpsData->gps_fix_acquired) {
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
                    if(appData->gpsData->gps_fix_acquired) {
                        gpsState = GPS_MODEM_FIX_ACQUIRED;
                    } else {
                        gpsState = GPS_MODEM_GET_FIX;
                    }
                    vTaskDelay(pdMS_TO_TICKS(1000));
                break;
                case GPS_MODEM_GET_FIX:
                    if (appData->modemMgr->GpsGetFix()) {
                        SerialMon.println("GPS fix acquired!");
                        appData->gpsData->gps_fix_acquired = true;
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
                    appData->gpsData->gps_fix_acquired = false;
                    vTaskDelay(pdMS_TO_TICKS(100));
                    break;
            }
            xSemaphoreGive(modemMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Main FreeRTOS task for cellular network management and SMS handling.
 * @param[in] pvParameters Pointer to task parameters (unused).
 */
void cellularTask(void* pvParameters) {
    sysAppData_t* appData = (sysAppData_t*)pvParameters;
    RegStatus status = REG_NO_RESULT;
    String SignalQuality;
    for (;;) {
        if (xSemaphoreTake(modemMutex, portMAX_DELAY) == pdTRUE) {
            if (appData->modemMgr->isSimReady() && appData->modemMgr->simSetNetworkMode(0) ) {
                String provider = appData->modemMgr->simGetOperator();
                SerialMon.println("Network provider: " + provider);
            } else {
                SerialMon.println("SIM not ready, cannot proceed.");
                xSemaphoreGive(modemMutex);
                vTaskDelay(pdMS_TO_TICKS(1000));
                continue;
            }

            if (status == REG_NO_RESULT || status == REG_SEARCHING || status == REG_UNREGISTERED) {
                 SerialMon.print("Wait for the modem to register with the network.");
                status = appData->modemMgr->simGetRegistrationStatus();
                switch (status) {
                    case REG_UNREGISTERED:
                    case REG_SEARCHING:
                        SignalQuality = appData->modemMgr->simGetSignalQuality();
                        SerialMon.printf("Not registered yet (Status: %d). Signal quality: %s\n", status, SignalQuality.c_str());
                        vTaskDelay(pdMS_TO_TICKS(1000));
                        break;
                    case REG_DENIED:
                        SerialMon.println("Network registration was rejected, please check if the APN is correct");
                        vTaskDelay(pdMS_TO_TICKS(100));
                        break;
                    case REG_OK_HOME:
                        SerialMon.println("Online registration successful");
                        break;
                    case REG_OK_ROAMING:
                        SerialMon.println("Network registration successful, currently in roaming mode");
                        break;
                    default:
                        SerialMon.printf("Registration Status:%d\n", status);
                        vTaskDelay(pdMS_TO_TICKS(100));
                        break;
                }
            }

            /* if registration is successful, start GPS, check for new SMS message and send GPS location if available */
            if (status == REG_OK_HOME || status == REG_OK_ROAMING) {
                String smsText = appData->modemMgr->simReadMessage();
                if( (smsText == SMS_REQ_LOCATION) && (appData->gpsData->gps_fix_acquired) ) {
                    SerialMon.println("Location request SMS received");
                    appData->cellData->msg_lat_buf = String(appData->gpsData->latitude, 6);
                    appData->cellData->msg_lon_buf = String(appData->gpsData->longitude, 6);
                    appData->cellData->msg_txt_sms = "Bike GPS Tracker position: " + String(GPS_MAP_URL) + appData->cellData->msg_lat_buf + "," + appData->cellData->msg_lon_buf;
                    SerialMon.println("Sending SMS to " + appData->cellData->target_number + ": " + appData->cellData->msg_txt_sms);
                    appData->modemMgr->simSendMessage(appData->cellData->target_number, appData->cellData->msg_txt_sms);
                }
            }
            xSemaphoreGive(modemMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(300));
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

    modemMutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(gpsTask, "GpsTask", GPS_TASK_STACK_SIZE, &sysAppData, GPS_TASK_PRIORITY, NULL, TASK_CORE_0);
    xTaskCreatePinnedToCore(cellularTask, "CellularTask", SMS_TASK_STACK_SIZE, &sysAppData, SMS_TASK_PRIORITY, NULL, TASK_CORE_1);
}

/*
 * @brief Arduino main loop. Not used, as logic is handled by FreeRTOS tasks.
 */
void loop() {
}