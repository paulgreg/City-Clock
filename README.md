# City-Clock

That project is a « fork » of the [city clock](https://www.instructables.com/id/The-Binary-Clock-the-City-Clock/), a binary clock.

That project uses an ESP8266 instead of an arduino.
The esp8266 connects to wifi and synchronise time via NTP at boot.
I’m also using a PCA9685 to control LEDs (I tried Charlieplexing but LEDs were too dim, the ESP8266 isn’t powerfull enought to light them)

It uses [NTPClient](https://github.com/arduino-libraries/NTPClient) for NTP and [Adafruit PWM Servo driver library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/).

A nice [page about GPIOs to use](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/).

## Connections

- ESP8266 VCC 3.3 V
- ESP8266 ground to ground
- ESP8266 CH_PD to 3.3 V
- ESP8266 GPIO4 on PCA9685 SDA
- ESP8266 GPIO5 on PCA9685 SDL
- PCA9685 VCC to 3.3 V
- PCA9685 ground to ground
- PCA9685 power connector to ground and VCC (to power up led)

And, to flash ESP8266 : 

- RX from FTDI to TX on ESP
- TX from FTDI to RX on ESP
- DTR from FTDI to GPIO 0 on ESP
- RST from FTDI to RESET on ESP