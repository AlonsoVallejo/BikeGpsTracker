## Bike GPS Tracker Project

This project is a GPS-based bike tracker using the LilyGo TTGO T-SIM7070G development board.  
The device acquires the bike's location via GPS and transmits it over the cellular network (SIM card with internet access) via SMS.

### Project Overview

- **Device:** LilyGo TTGO T-SIM7070G (ESP32 + SIM7070G modem)
- **Functionality:**
    - Acquire GPS location
    - Transmit location data via SMS over cellular network
    - Real-time bike tracking
    - Bluetooth connection to Android app for setting SMS target number
    - (Optional) Save GPS location history to SD card

### Hardware Requirements

- LilyGo TTGO T-SIM7070G board
- SIM card with active data plan
- GPS antenna (included with board)
- USB cable for programming and power
- (Optional) MicroSD card for data logging

### Implementation Details

- **FreeRTOS Tasks:**  
  Two main tasks run in parallel:
  - `gpsTask`: Handles GPS acquisition and reporting. Takes exclusive access to the modem using a mutex while acquiring a GPS fix.
  - `cellularTask`: Manages cellular network registration, checks SIM status, sets network mode, receives SMS requests, and sends location via SMS. Also uses the mutex to ensure GNSS and GPRS are not accessed simultaneously.

- **SIM7070G Limitations:**  
  The modem cannot use GNSS (GPS) and GSM/LTE (cellular) functions at the same time. Tasks are synchronized using a FreeRTOS mutex to avoid conflicts.

- **SMS Location Requests:**  
  When an SMS with the text "LOCATION" is received, the device replies with a Google Maps URL containing the current latitude and longitude.

- **Network Provider:**  
  The current mobile network provider is detected and printed after SIM initialization and registration.

### Getting Started

1. Assemble the TTGO T-SIM7070G board with GPS antenna and SIM card.
2. Connect the board to your computer via USB.
3. Flash the firmware using PlatformIO.
4. Monitor serial output for GPS location, network status, and SMS events.

### Notes

- The SIM7070G module requires time-multiplexing between GNSS and cellular functions due to shared RF hardware.
- All modem operations are protected by a FreeRTOS mutex to prevent simultaneous access.

### For More Details

See the `src/main.cpp` file and the documentation in the `doc/` folder for implementation and usage details.
