#define SerialMon Serial
#define SerialAT Serial1

/* lillyGo SIM7070G shield pin definitions */
#define MODEM_UART_BAUD   9600
#define MODEM_PIN_DTR     25
#define MODEM_PIN_TX      27
#define MODEM_PIN_RX      26
#define MODEM_PWR_PIN     4

#define USER_BLUE_LED_PIN 12
#define TURN_OFF_LED() digitalWrite(USER_BLUE_LED_PIN, HIGH)
#define TURN_ON_LED()  digitalWrite(USER_BLUE_LED_PIN, LOW)
#define TOGGLE_LED()   digitalWrite(USER_BLUE_LED_PIN, !digitalRead(USER_BLUE_LED_PIN))