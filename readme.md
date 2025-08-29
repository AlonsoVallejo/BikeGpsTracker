## Bike GPS Tracker Project

This project is a GPS-based bike tracker using the LilyGo TTGO T-SIM7070G development board. 
The device acquires the bike's location via GPS and transmits it over the cellular network (SIM card with internet access) to a future user interface web application (pending development).

### Project Overview

- **Device:** LilyGo TTGO T-SIM7070G (ESP32 + SIM7070G modem)
- **Functionality:**
	- Acquire GPS location
	- Transmit location data via cellular network
	- Designed for real-time bike tracking
	- Future integration with a web app for user access

### Hardware Requirements (Current)

- LilyGo TTGO T-SIM7070G board
- SIM card with active data plan
- GPS antenna (included with board)
- USB cable for programming and power
- (Optional) MicroSD card for data logging

### Status

- Hardware setup and basic GPS/cellular communication implemented
- Web app for user interface: **pending**

### Getting Started

1. Assemble the TTGO T-SIM7070G board with GPS antenna and SIM card.
2. Connect the board to your computer via USB.
3. Flash the firmware using PlatformIO.
4. Monitor serial output for GPS location and AT command interface.

For more details, see the `src/main.cpp` file and the documentation in the `doc/` folder.
