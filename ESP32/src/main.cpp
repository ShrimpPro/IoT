#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* WIFI_SSID = "<WIFI_SSID>";
const char* WIFI_PASS = "<WIFI_PASS>";

String GET_ENDPOINT = "<GET_ENDPOINT>";

unsigned long currentTime = 0;
unsigned long requestDelay = 5000;

void setup() {
  Serial.begin(115200); 

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Wifi connected, local address:");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((millis() - currentTime) > requestDelay) {
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = GET_ENDPOINT;
      http.begin(serverPath.c_str());
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    currentTime = millis();
  }
}