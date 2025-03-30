#include "parameters.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <NTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
#include <ESP8266httpUpdate.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", TIMEZONE * 3600, NTP_UPDATE_INTERVAL);

unsigned int LEDs = 13; // the number of LEDs

unsigned int hours1, hours2;
unsigned int minutes1, minutes2;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); // Default address 0x40

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("\n\nCity Clock");

  pwm.begin();
  pwm.setPWMFreq(1600);

  turnAllLedsOff();
  if (!DEBUG_ALL_LED_ON) connectToWifi();

  turnAllLedsOn();
  while (DEBUG_ALL_LED_ON) delay(10000);

  checkForUpdate();

  unsigned int summerTime = checkIsSummerTime(PARAM_URL);
  Serial.print("summerTime: "); Serial.println(summerTime);
  timeClient.setTimeOffset((TIMEZONE + summerTime) * 3600);

  Serial.println("Initializing NTP");
  timeClient.begin();
  delay(100);
  timeClient.update();
  
  turnAllLedsOff();
}

void loop() {
  // First update LEDS
  Serial.println(timeClient.getFormattedTime());  
  int hours = timeClient.getHours();
  hours1 = hours / 10;
  hours2 = hours % 10;
  
  int minutes = timeClient.getMinutes();
  minutes1 = minutes / 10;
  minutes2 = minutes % 10;

  handleAllLEDs();

  // then, eventually reconnect to wifi and update NTP
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi lost");
    connectToWifi();
    timeClient.begin();
    delay(100);
  }
  timeClient.update();

  // then, sleep until next minute
  int waitUntilNextMinute = (60 - timeClient.getSeconds()) * 1000;
  Serial.print("Sleeping for ");
  Serial.print(waitUntilNextMinute / 1000);
  Serial.print(" seconds\n\n");
  delay(waitUntilNextMinute + 500);

}

inline void handleAllLEDs() {
  handleLED( 0, hours1 & 2);
  handleLED( 1, hours1 & 1);

  handleLED( 2, hours2 & 8);
  handleLED( 3, hours2 & 4);
  handleLED( 4, hours2 & 2);
  handleLED( 5, hours2 & 1);

  handleLED( 6, minutes1 & 4);
  handleLED( 7, minutes1 & 2);
  handleLED( 8, minutes1 & 1);

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
}

inline void turnLedOff(int l) {
  handleLED(l, false);
}

inline void turnAllLedsOn() {
  for (int l = 0; l < LEDs; l++) turnLedOn(l);
}

inline void turnAllLedsOff() {
  for (int l = 0; l < LEDs; l++) turnLedOff(l);
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
    delay(750);
    turnLedOff(l);
  }
  Serial.println("\nwifi connected\nIP address:");
  Serial.println(WiFi.localIP());
  return true;
}

void checkForUpdate() {
  String urlversion = String(UPDATE_URL);
  urlversion.concat("version.txt");
  Serial.print("\nCheck version at ");
  Serial.println(urlversion);
  HTTPClient httpClient;
  httpClient.setReuse(true);
  httpClient.begin(urlversion);
  int httpCode = httpClient.GET();

  if(httpCode == HTTP_CODE_OK) {
    turnAllLedsOff();
    turnLedOn(3);
    String nVersion = httpClient.getString();
    Serial.print("Actual version: ");
    Serial.println(VERSION);
    Serial.print( "Available version: ");
    nVersion.trim();
    Serial.println(nVersion.toInt());
    if(nVersion.toInt() > VERSION) {
      turnLedOn(4);
      Serial.print("Update available: ");
      String urlfirmware = String(UPDATE_URL);
      urlfirmware.concat("firmware.");
      urlfirmware.concat(nVersion);
      urlfirmware.concat(".bin");
      Serial.println(urlfirmware);
      Serial.println("Updating...");
      turnLedOn(5);
      t_httpUpdate_return ret = ESPhttpUpdate.update(urlfirmware);
      // reboot if update is successful
      turnLedOn(6);
      if(ret == HTTP_UPDATE_FAILED) {
        Serial.print("Update error (");
        Serial.print(ESPhttpUpdate.getLastError());
        Serial.print(") : ");
        Serial.println(ESPhttpUpdate.getLastErrorString().c_str());
      }
    } else {
      Serial.println("Already to latest version");
    }
  } else {
    Serial.print("Error while fetching version on server :");
    Serial.println(httpCode);
  }
}


unsigned int checkIsSummerTime(const char* url) {
  WiFiClientSecure client;
  client.setInsecure();
  
  Serial.print("Connecting to ");
  Serial.println(url);
  
  turnAllLedsOff();
  HTTPClient httpClient;
  httpClient.setReuse(true);
  httpClient.begin(client, url);
  int httpCode = httpClient.GET();
  Serial.print("Status code:"); Serial.println(httpCode);

  if(httpCode != HTTP_CODE_OK) {
    turnLedOn(7);
    Serial.print("Error on HTTP request : ");
    Serial.println(httpCode);
  } else {
    turnLedOn(8);
    String s = httpClient.getString();
    return s.indexOf("SUMMER=1") == 0 ? 1 : 0;

  }
  httpClient.end();

  return false;
}
