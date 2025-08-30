#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "modemMgr.h"

/*
 * @brief ModemMgr constructor
 * @paramin modem Reference to TinyGsm modem object
 * @paramin serialMon Reference to Serial monitor
 * @paramin serialAT Reference to Serial AT port
 * @paramin pwrPin Modem power control pin
 * @paramin dtrPin Modem DTR (sleep/wake) pin
 */
ModemMgr::ModemMgr(TinyGsm& modem, HardwareSerial& serialMon, HardwareSerial& serialAT, int pwrPin, int dtrPin)
    : modem(modem), serialMon(serialMon), serialAT(serialAT), pwrPin(pwrPin), dtrPin(dtrPin) {}

/*
 * @brief Power on the modem by toggling the power pin sequence.
 */
void ModemMgr::powerOn() {
    /* Ensure pin is LOW before starting power sequence */
    serialMon.println("Powering on modem...");
    digitalWrite(pwrPin, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(pwrPin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(pwrPin, LOW);
}

/*
 * @brief Power off the modem by toggling the power pin sequence.
 */
void ModemMgr::powerOff() {
    /* Pull HIGH for required time to power off modem */
    serialMon.println("Powering off modem...");
    digitalWrite(pwrPin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1500));
    digitalWrite(pwrPin, LOW);
}

/*
 * @brief Restart the modem by powering off and then on.
 */
void ModemMgr::restart() {
    powerOff();
    vTaskDelay(pdMS_TO_TICKS(1000));
    powerOn();
}

/*
 * @brief Test AT communication with the modem.
 * @return true if modem responds to AT, false otherwise.
 */
bool ModemMgr::test() {
    if (!modem.testAT()) {
        serialMon.println("Failed to communicate with modem");
        return false;
    }
    serialMon.println("Modem is ready");
    return true;
}

/*
 * @brief Wake up the modem by toggling the DTR pin.
 */
void ModemMgr::awake() {
    /* Pull DTR HIGH to wake modem */
    digitalWrite(dtrPin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    /* Pull DTR LOW to complete wake sequence */
    digitalWrite(dtrPin, LOW);
}
