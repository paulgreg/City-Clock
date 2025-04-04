# City-Clock

That project is a « fork » of the [city clock](https://www.instructables.com/id/The-Binary-Clock-the-City-Clock/), a binary clock.

That project uses an ESP8266 instead of an arduino.
You should use esp8266 2.x.x to flash.

The esp8266 connects to wifi and synchronise time via NTP at boot.
I’m also using a PCA9685 to control LEDs (I tried Charlieplexing but LEDs were too dim, the ESP8266 isn’t powerfull enought to light them)

It uses [NTPClient](https://github.com/arduino-libraries/NTPClient) for NTP and [Adafruit PWM Servo driver library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/).

It expects a file on `PARAM_URL` containing `SUMMER=0` or `SUMMER=1` to configure summer time.

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


## Update via LAN

You can update the code via wifi.
When esp8266 is booting, it will check at `UPDATE_URL` for a `version.txt` file which should only contain a number.
That number is compared with `VERSION` value in parameters.h and, if greater, it will try to download `firmware.VERSION.bin` (where VERSION should be the same as the value in `version.txt`).

To update via wifi, first increment the `VERSION` and generate binary from « Sketch > Export compiled binary » in arduino menu. That create a binary file in current directory.

Then, create a new directory on your PC, create a `version.txt` file with the same value as `VERSION`. Move the generated .bin file to that directory and rename it `firmware.VERSION.bin` (with same version number).

Then start a web server in that directory, for exemple by using that node server :

    npx http-server -p 80 .

Please note that you may need to allow node to open port 80 :

    sudo setcap cap_net_bind_service=+ep `which node`

Reboot the esp8266 and it should connect to `UPDATE_URL`. You can watch at server log to see it it’s connecting.
