#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* DEVICE_ID = "<DEVICE_ID>";

const char* WIFI_SSID = "iiFi-NFT";
const char* WIFI_PASS = "PW12398799";

String SERVER_URL = "https://httpbin.org";

unsigned long currentTime = 0;
unsigned long requestDelay = 3000;

const int oneWireBus = 4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

float phPin = 36;
float PH4 = 2.1;
float PH7 = 1.64;

void getRequest();
void postRequest(float ph, float temp);
void checkHttpConnection(int httpResponseCode, String payload);
float senseTemp();
float sensePH();

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
      getRequest();
      float ph = sensePH();
      float temp = senseTemp();
      postRequest(ph, temp);
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

void postRequest(float ph, float temp) {
      String getPath = SERVER_URL + "/post";
      HTTPClient http;

      Serial.println("Begin connection \n");
      http.begin(getPath.c_str());

      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestPostData = (String) "DeviceId=" + DEVICE_ID
        + "&temperature=" + temp
        + "&pH=" + ph;
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

float senseTemp() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println("ºC");
  return temperatureC;
  // delay(5000);
}

float sensePH() {
  int analogValue = analogRead(phPin);
  double phVoltage = 3.3 / 4095.0 * analogValue;
  float phCallib = (PH4 - PH7) / 3;
  float pH = 7.00 + ((PH7 - phVoltage) / phCallib);
  Serial.print("PH: ");
  Serial.print(pH);
  Serial.print(" | Tegangan: ");
  Serial.print(phVoltage);
  Serial.print(" | ADC: ");
  Serial.println(analogValue);
  return pH;
  // delay(2000)
}