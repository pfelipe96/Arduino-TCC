#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

char ssidRouter[] = "dlink-F854";
char passwordRouter[] = "cdone55602";

const int firstCapactiveSensor = 5;
const int secondCapactiveSensor = 4;
const int thirdCapactiveSensor = 0;

const int firstFloaterSensor = 14;
const int secondFloaterSensor = 12;
const int thirdFloaterSensor = 13;

boolean isEnableFirstWaterLevel = false;
boolean isEnableSecondWaterLevel = false;
boolean isEnableThirdWaterLevel = false;

HTTPClient http;

const char* host = "https://us-central1-tcc-2-33e61.cloudfunctions.net/saveLevelStation";

const int capacity = 3 * JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> payloadJson;
JsonObject stationJson = payloadJson.createNestedObject("station");
    
WiFiClientSecure client;


void setup() {
  Serial.begin(9600);

  Serial.println("Setup");
  
  setupConnectionToInternet();
  setupPayloadToRequest();

  pinMode(firstCapactiveSensor, INPUT_PULLUP);
  pinMode(secondCapactiveSensor, INPUT_PULLUP);
  pinMode(thirdCapactiveSensor, INPUT_PULLUP);

  pinMode(firstFloaterSensor, INPUT);
  pinMode(secondFloaterSensor, INPUT);
  pinMode(thirdFloaterSensor, INPUT);
}


void setupConnectionToInternet(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssidRouter);
  WiFi.begin(ssidRouter, passwordRouter);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  client.setInsecure();
  client.connect(host, 443);
}

void setupPayloadToRequest(){
  payloadJson["token"] = "9a3e0058a226d3074a68a29813fc136b";
  payloadJson["level"] = 0;
  stationJson["name"] = "Estação João Dias 01";
  stationJson["address"] = "Avenida João Dias 2025";
  stationJson["id"] = "01";
}

void loop() {   
  setupConnectionToHttps();
  checkIfAllPairSensorsDisabled();
  checkIfFirstPairSensorsEnabled();
  checkIfSecondPairSensorsEnabled();
  checkIfThirdPairSensorsEnabled();

  delay(3000);
}

void setupConnectionToHttps(){
   if (WiFi.status() == WL_CONNECTED) {
    String json;

    http.begin(client, host);
    http.addHeader("Content-Type", "application/json");

    serializeJson(payloadJson, json);
    
    Serial.println(json);
    
    int httpCode = http.POST(json);
    String payload = http.getString();
 
    Serial.println(httpCode);
    Serial.println(payload);
 
    http.end();

  } else {
    Serial.println("Error in WiFi connection");
  }
}

void checkIfAllPairSensorsDisabled(){
  if(digitalRead(firstCapactiveSensor) == HIGH && digitalRead(firstFloaterSensor) == LOW && isEnableFirstWaterLevel == true){
     payloadJson["level"] = 0;
     setWaterNivel(false, false, false);
  }
}

void checkIfFirstPairSensorsEnabled(){
  if(digitalRead(firstCapactiveSensor) == LOW && digitalRead(firstFloaterSensor) == HIGH){
     payloadJson["level"] = 1;
     setWaterNivel(true, false, false);
  }
}

void checkIfSecondPairSensorsEnabled(){
  if(digitalRead(secondCapactiveSensor) == LOW && digitalRead(secondFloaterSensor) == HIGH){
      payloadJson["level"] = 2;
      setWaterNivel(false, true, false);
  }
}

void checkIfThirdPairSensorsEnabled(){
  if(digitalRead(thirdCapactiveSensor) == LOW && digitalRead(thirdFloaterSensor) == HIGH){
      payloadJson["level"] = 3;
      setWaterNivel(false, false, true);
  }
}

void setWaterNivel(boolean firstLevel, boolean secondLevel, boolean thirdLevel){
    isEnableFirstWaterLevel = firstLevel;
    isEnableSecondWaterLevel = secondLevel;
    isEnableThirdWaterLevel = thirdLevel;
}
