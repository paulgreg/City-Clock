#include "parameters.h"
#include <ESP8266WiFi.h>

#include <NTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>

#define DEBUG true

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup() {
  Serial.begin(74880);
  while (!Serial);
  Serial.println("City Clock");
  
  if (connectToWifi()) {
    timeClient.begin();
  }
}

void loop() {
  timeClient.update();
  if (DEBUG) Serial.println(timeClient.getFormattedTime());

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
