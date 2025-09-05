#pragma once
#include <Arduino.h>
#define TINY_GSM_MODEM_SIM7070
#define TINY_GSM_RX_BUFFER 1024
#include <TinyGsmClient.h>

#define SerialMon Serial
#define SerialAT Serial1

#define MODEM_UART_BAUD   9600
#define MODEM_PIN_DTR     25
#define MODEM_PIN_TX      27
#define MODEM_PIN_RX      26
#define MODEM_PWR_PIN     4

class ModemMgr {
public:
    ModemMgr(TinyGsm& modem, HardwareSerial& serialMon, HardwareSerial& serialAT, int pwrPin, int dtrPin);
    
    /* General SIM7070G modem functions */
    void init();
    void powerOn();
    void powerOff();
    void restart();
    bool test();
    void awake();

    /* SIM7070G GPS functions */
    void GpsEnable();
    void GpsDisable();
    bool GpsGetFix();
    void GpsGetLatLon(float* lat, float* lon);

    /* SIM7070G cellular functions */
protected:
    TinyGsm& modem;
    HardwareSerial& serialMon;
    HardwareSerial& serialAT;
    int pwrPin;
    int dtrPin;
};

