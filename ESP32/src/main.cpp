#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FirebaseESP32.h>

#define USER_EMAIL 
#define USER_PASSWORD 
#define WIFI_SSID 
#define WIFI_PASS 
#define FIREBASE_HOST 
#define FAPI_KEY 

String POND_ID = "6425db4ba1202bb6288f31d9";
String POND_ID2 = "6427e5c3a1202bb62800237b";
String DEVICE_ID = "6425d846a1202bb62882fe7d";
String DEVICE_ID2 = "6427e750a1202bb628058124";
String SERVER_URL = "https://shrimpro.seirei.site/iot/devices";

FirebaseData fbdo;
FirebaseAuth fauth;
FirebaseConfig fconfig;

unsigned long currentTime = 0;
unsigned long requestDelay = 30000;
unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;
bool signupOK = false;

const int oneWireBus = 4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

float phPin = 36;
float PH4 = 2.1;
float PH7 = 1.64;

void firebaseConnect(float ph, float temp);
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

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  fconfig.host = FIREBASE_HOST;
  fconfig.api_key = FAPI_KEY;
  fauth.user.email = USER_EMAIL;
  fauth.user.password = USER_PASSWORD;

   if (Firebase.signUp(&fconfig, &fauth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", fconfig.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */

  Firebase.begin(&fconfig, &fauth);
  Firebase.reconnectWiFi(false);
}

void loop() {
    float ph = sensePH();
    float temp = senseTemp();
  if ((millis() - currentTime) > requestDelay) {
    if(WiFi.status()== WL_CONNECTED){
      // getRequest();
      postRequest(ph, temp);
      firebaseConnect(ph, temp); 
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
      String getPath = SERVER_URL + "/" + POND_ID;
      HTTPClient http;

      Serial.println("Begin connection \n");
      http.begin(getPath.c_str());

      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestPostData = (String) "DeviceId=" + DEVICE_ID
        + "&temp=" + temp
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

void firebaseConnect(float ph, float temp) {
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.setFloat(&fbdo, POND_ID + "/pH", ph)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setFloat(&fbdo, POND_ID + "/temp", temp)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setString(&fbdo, POND_ID + "/DeviceId", DEVICE_ID)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    //This one is dummy
    if (Firebase.RTDB.setFloat(&fbdo, POND_ID2 + "/pH", ph + 2)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setFloat(&fbdo, POND_ID2 + "/temp", temp + 2)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setString(&fbdo, POND_ID2 + "/DeviceId", DEVICE_ID2)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

