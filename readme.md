## Bike GPS Tracker Project

This project is a GPS-based bike tracker using the LilyGo TTGO T-SIM7070G development board. 
The device acquires the bike's location via GPS and transmits it over the cellular network (SIM card with internet access) via SMS.

### Project Overview

- **Device:** LilyGo TTGO T-SIM7070G (ESP32 + SIM7070G modem)
- **Functionality:**
	- Acquire GPS location
	- Transmit location data via cellular network
	- Designed for real-time bike tracking
	- Bluetooth connection between android app and device to indicate SMS target number. 
	- Data about GPS location history will be saved into SDcard (Optional).

### Hardware Requirements (Current)

- LilyGo TTGO T-SIM7070G board
- SIM card with active data plan
- GPS antenna (included with board)
- USB cable for programming and power
- (Optional) MicroSD card for data logging

### Status

- Hardware setup and basic GPS/cellular communication implemented
- Web app for user interface: **pending**

### Current bugs detected:

- ESP32 is being reset after passing the interval to get the GPS fix again. This appears to be related to a task watchdog timeout when reacquiring a GPS fix. See log below:

```
Waiting for GPS fix...
AT+CGNSINF
AT+CGNSINF
+CGNSINF: 1,1,20250830031120.000,20.563343,-103.433201,1597.811,0.00,6.2,0,,1.5,1.8,1.0,,8,,27.0,38.2
OK
GPS fix acquired!
AT+CGNSINF
AT+CGNSINF
+CGNSINF: 1,1,20250830031121.000,20.563347,-103.433195,1593.933,0.00,6.2,0,,1.5,1.8,1.0,,8,,26.4,37.0
OK
Latitude: 20.563347, Longitude: -103.433197
Disabling GPS...
AT+CGPIO=0,48,1,0
AT+CGPIO=0,48,1,0
OK
AT+CGNSPWR=0
AT+CGNSPWR=0
OK
Entering idle state, will reacquire GPS fix after interval
AT
AT
OK
Modem is ready
AT+CGPIO=0,48,1,1
AT+CGPIO=0,48,1,1
OK
AT+CGNSPWR=1
AT+CGNSPWR=1
OK
Start GPS positioning!
AT+CGNSINF
E (50354) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (50354) task_wdt:  - IDLE0 (CPU 0)
E (50354) task_wdt: Tasks currently running:
E (50354) task_wdt: CPU 0: GpsTask
E (50354) task_wdt: CPU 1: loopTask
E (50354) task_wdt: Aborting.

abort() was called at PC 0x400dae31 on core 0

Backtrace: 0x40083459:0x3ffbea0c |<-CORRUPTED
```

### Notes

- The SIM7070G module has a hardware limitation where it cannot operate its GSM/LTE and GNSS (GPS) functions simultaneously because they share RF components, requiring time-multiplexing.

### Getting Started

1. Assemble the TTGO T-SIM7070G board with GPS antenna and SIM card.
2. Connect the board to your computer via USB.
3. Flash the firmware using PlatformIO.
4. Monitor serial output for GPS location and AT command interface.

For more details, see the `src/main.cpp` file and the documentation in the `doc/` folder.
