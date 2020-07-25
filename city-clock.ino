#include "parameters.h"
#include <ESP8266WiFi.h>

#include <NTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", (TIMEZONE + SUMMERTIME) * 3600, 60000);

int LEDs = 13; // the number of LEDs

int hours1, hours2;
int minutes1, minutes2;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); // Default address 0x40

void testEachLEDs() {
  for (int l = 0; l < LEDs; l++) {
    Serial.print("Testing LED ");
    Serial.println(l);
    turnLedOn(l);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("City Clock");

  pwm.begin();
  pwm.setPWMFreq(1600);

  while (TEST_LED) testEachLEDs();

  delay(500);
  if (connectToWifi()) {
    timeClient.begin();
  }
  Serial.println("End setup");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi lost");
    connectToWifi();
    timeClient.begin();
  }
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  
  int hours = timeClient.getHours();
  hours1 = hours / 10;
  hours2 = hours % 10;
  
  int minutes = timeClient.getMinutes();
  minutes1 = minutes / 10;
  minutes2 = minutes % 10;

  handleAllLEDs();
  
  int waitUntilNextMinute = (60 - timeClient.getSeconds()) * 1000;
  delay(waitUntilNextMinute);
}

inline void handleAllLEDs() {
  handleLED(0, hours1 & 2);
  handleLED(1, hours1 & 1);

  handleLED(2, hours2 & 8);
  handleLED(3, hours2 & 4);
  handleLED(4, hours2 & 2);
  handleLED(5, hours2 & 1);

  handleLED(6, minutes1 & 4);
  handleLED(7, minutes1 & 2);
  handleLED(8, minutes1 & 1);

  handleLED( 9, minutes2 & 8);
  handleLED(10, minutes2 & 4);
  handleLED(11, minutes2 & 2);
  handleLED(12, minutes2 & 1);
}

inline void handleLED(int i, boolean test) {
  int maxValue = 4096;
  pwm.setPWM(i, test ? maxValue : 0, test ? 0 : maxValue);
}

inline void turnLedOn(int l) {
  handleLED(l, true);
  delay(1000);
  handleLED(l, false);
}

boolean connectToWifi() {
  Serial.print("\nconnecting to ");
  Serial.println(WIFI_SSID);

  WiFi.forceSleepWake();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    int l = retries++ % LEDs;
    turnLedOn(l);
  }
  Serial.println("");
  Serial.println("wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  return true;
}
