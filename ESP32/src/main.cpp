#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* DEVICE_ID = "<DEVICE_ID>";

const char* WIFI_SSID = "<WIFI_SSID>";
const char* WIFI_PASS = "<WIFI_PASS>";

String SERVER_URL = "<SERVER_URL>";

unsigned long currentTime = 0;
unsigned long requestDelay = 30000;

const int oneWireBus = 4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void getRequest();
void postRequest();
void checkHttpConnection(int httpResponseCode, String payload);

void setup() {
  Serial.begin(115200); 
  sensors.begin();

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
      // getRequest();
      postRequest();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    currentTime = millis();
  }
}

void getRequest() {
      String getPath = SERVER_URL + "/get";
      HTTPClient http;

      Serial.println("Begin connection \n");
      http.begin(getPath.c_str());

      int httpResponseCode = http.GET();
      String payload = http.getString();

      checkHttpConnection(httpResponseCode, payload);

      Serial.println("End connection \n");
      // http.end();
}

void postRequest() {
      String getPath = SERVER_URL + "/post";
      HTTPClient http;

      Serial.println("Begin connection \n");
      http.begin(getPath.c_str());

      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestPostData = (String) "DeviceId=" + DEVICE_ID
        + "&temperature=" + 22.00
        + "&pH=" + 6;
      int httpResponseCode = http.POST(httpRequestPostData);
      String payload = http.getString();

      checkHttpConnection(httpResponseCode, payload);

      Serial.println("End connection \n");
      // http.end();
}

void checkHttpConnection (int httpResponseCode, String payload) {
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
}


void temperatureSensor() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");
  delay(5000);
}