#include "parameters.h"
#include <ESP8266WiFi.h>

#include <NTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
#include <ChuckPlex.h> // https://github.com/marcuserronius/ChuckPlex

#define DEBUG true

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

int pins[] = {5, 4, 15}; // the pins you'll be using
int pinCount = 3;
int nodes = 4; // the number of LEDs
ChuckPlex plex = ChuckPlex(pins,pinCount);

void setup() {
  Serial.begin(74880);
  while (!Serial);
  Serial.println("City Clock");

  if (DEBUG) plex.displayConnections(nodes);
  
  if (connectToWifi()) {
    timeClient.begin();
  }
}

void loop() {
  timeClient.update();
  if (DEBUG) Serial.println(timeClient.getFormattedTime());

  int led = timeClient.getSeconds() % 4;

  if (DEBUG) {
    Serial.print("enabling ");
    Serial.println(led);
  }

  plex.clear();
  plex.enable(led + 1);
  
  delay(1000);
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
  unsigned int retries = 100;
  while (WiFi.status() != WL_CONNECTED && (retries-- > 0)) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWifi connection failed");
    return false;
  }
  Serial.println("");
  Serial.println("wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  return true;
}
