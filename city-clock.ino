#include "parameters.h"
#include <ESP8266WiFi.h>

#include <NTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
#include <ChuckPlex.h> // https://github.com/marcuserronius/ChuckPlex

#define DEBUG true

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

int pins[] = { 5, 4, 14, 12, 13 }; // the pins you'll be using
int pinCount = 5;
int LEDs = 13; // the number of LEDs

ChuckPlex plex = ChuckPlex(pins,pinCount);

void setup() {
  Serial.begin(74880);
  while (!Serial);
  Serial.println("City Clock");

  if (DEBUG) plex.displayConnections(LEDs);

  for (int i = 1; i < 14; i++) {
    Serial.print("Testing LED # ");
    Serial.println(i);
    plex.enable(i);
    delay(500);
  }
  plex.clear();

  if (connectToWifi()) {
    timeClient.begin();
  }
}

unsigned int c = 0;
unsigned int hours1, hours2;
unsigned int minutes1, minutes2;  

void loop() {
  if (c++ % 10000 == 0) {
    timeClient.update();
  
    int hours = timeClient.getHours();
    hours1 = hours / 10;
    hours2 = hours % 10;
    
    int minutes = timeClient.getMinutes();
    minutes1 = minutes / 10;
    minutes2 = minutes % 10;
  
    if (DEBUG) {
      Serial.print(hours);
      Serial.print(":");
      Serial.print(minutes);
      Serial.print(":");
      Serial.println(timeClient.getSeconds());
    }
  }

  turnLedOn(1, hours1 & 1);
  turnLedOn(2, hours1 & 2);

  turnLedOn(3, hours2 & 1);
  turnLedOn(4, hours2 & 2);
  turnLedOn(5, hours2 & 4);
  turnLedOn(6, hours2 & 8);

  turnLedOn(7, minutes1 & 1);
  turnLedOn(8, minutes1 & 2);
  turnLedOn(9, minutes1 & 4);

  turnLedOn(10, minutes2 & 1);
  turnLedOn(11, minutes2 & 2);
  turnLedOn(12, minutes2 & 4);
  turnLedOn(14, minutes2 & 8);
}

void turnLedOn(int i, boolean test) {
  if (test) plex.enable(i);
}

boolean connectToWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nAlready connected");
    return true;
  }
  Serial.print("\nconnecting to ");
  Serial.println(WIFI_SSID);

  WiFi.forceSleepWake();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    plex.clear();
    plex.enable((retries++ % LEDs) + 1);
    delay(500);
  }
  Serial.println("");
  Serial.println("wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  return true;
}
