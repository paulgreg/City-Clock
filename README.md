# City-Clock

That project is a « fork » of the [city clock](https://www.instructables.com/id/The-Binary-Clock-the-City-Clock/), a binary clock.

That project uses an ESP8266 instead of an arduino.
The esp8266 connects to wifi and synchronise time via NTP at boot.

It uses [NTPClient](https://github.com/arduino-libraries/NTPClient) for NTP and
[ChuckPlex](https://github.com/marcuserronius/ChuckPlex) for Charlieplexing  (to connect more led than GPIO available).

A nice [page about GPIOs to use](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/).


## Connections to flash ESP

- VCC to 3.3
- Ground to ground
- CH_PD from ESP to VCC
- RX from FTDI to TX on ESP
- TX from FTDI to RX on ESP
- DTR from FTDI to GPIO 0 on ESP
- RST from FTDI to RESET on ESP
