#include "parameters.h"
#include <ESP8266WiFi.h>

#include <NTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
#include <ChuckPlex.h> // https://github.com/marcuserronius/ChuckPlex

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

int pins[] = { 5, 4, 14, 12, 13 }; // the pins you'll be using
int pinCount = 5;
int LEDs = 13; // the number of LEDs

ChuckPlex plex = ChuckPlex(pins,pinCount);

int hours1, hours2;
int minutes1, minutes2;

unsigned int c = 0;

#if DEBUG
  #define NB_LOOP 10000
#else     
  #define NB_LOOP 15000
#endif    

void setup() {
  Serial.begin(74880);
  while (!Serial);
  Serial.println("City Clock");

  if (DEBUG) plex.displayConnections(LEDs);

  for (int i = 1; i <= LEDs; i++) {
    Serial.print("Testing LED # ");
    Serial.println(i);
    plex.enable(i);
    delay(500);
  }
  plex.clear();

  delay(500);
  if (connectToWifi()) {
    timeClient.begin();
  }
  plex.clear();
  Serial.println("End setup");
}

void loop() {
  if (c++ % NB_LOOP == 0) {
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
  turnAllLedsOn();
}

inline void turnAllLedsOn() {
  turnLedOn(1, hours1 & 2);
  turnLedOn(2, hours1 & 1);

  turnLedOn(3, hours2 & 8);
  turnLedOn(4, hours2 & 4);
  turnLedOn(5, hours2 & 2);
  turnLedOn(6, hours2 & 1);

  turnLedOn(7, minutes1 & 4);
  turnLedOn(8, minutes1 & 2);
  turnLedOn(9, minutes1 & 1);

  turnLedOn(10, minutes2 & 8);
  turnLedOn(11, minutes2 & 4);
  turnLedOn(12, minutes2 & 2);
  turnLedOn(13, minutes2 & 1);
}

inline void turnLedOn(int i, boolean test) {
  if (test) plex.enable(i);
}

boolean connectToWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nAlready connected");
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
    delay(800);
  }
  Serial.println("");
  Serial.println("wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  return true;
}
