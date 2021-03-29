#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

char ssid[] = "dlink-F854";
char password[] = "cdone55602";

const int pinoIVReflexivo1 = 16;
const int pinoIVReflexivo2 = 5;
const int pinoIVReflexivo3 = 4;

const int pinoLedToIV1 = 0;
const int pinoLedToIV2 = 2;
const int pinoLedToIV3 = 14;

HTTPClient http;

const char* host = "https://us-central1-tcc-2-33e61.cloudfunctions.net/saveLevelStation";

const int capacity = 3 * JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> payloadJson;
JsonObject stationJson = payloadJson.createNestedObject("station");

const char fingerprint[] PROGMEM =  "48 50 4E 97 4C 0D AC 5B 5C D4 76 C8 20 22 74 B2 4C 8C 71 72";

void setup() {
  Serial.begin(9600);

  Serial.println("Setup");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
      
  payloadJson["token"] = "9a3e0058a226d3074a68a29813fc136b";
  payloadJson["level"] = 0;
  stationJson["name"] = "Estação João Dias 01";
  stationJson["address"] = "Avenida João Dias 2025";
  stationJson["id"] = "01";

  pinMode(pinoIVReflexivo1, INPUT);
  pinMode(pinoLedToIV1, OUTPUT);
  digitalWrite(pinoLedToIV1, LOW);

  pinMode(pinoIVReflexivo2, INPUT);
  pinMode(pinoLedToIV2, OUTPUT);
  digitalWrite(pinoLedToIV2, LOW);

  pinMode(pinoIVReflexivo3, INPUT);
  pinMode(pinoLedToIV3, OUTPUT);
  digitalWrite(pinoLedToIV3, LOW);
}


void loop() {   
  if (WiFi.status() == WL_CONNECTED) {
    String json;
    
    WiFiClientSecure client;
    client.setInsecure(); //the magic line, use with caution
    client.connect(host, 443);

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
  
  if(digitalRead(pinoIVReflexivo1) == LOW){
      digitalWrite(pinoLedToIV1, HIGH);
      payloadJson["level"] = 1;
  }else{ 
    digitalWrite(pinoLedToIV1, LOW);
    payloadJson["level"] = 0;
  }

  if(digitalRead(pinoIVReflexivo2) == LOW){
      digitalWrite(pinoLedToIV2, HIGH);
      payloadJson["level"] = 2;
  }else{ 
    digitalWrite(pinoLedToIV2, LOW);
  }

  if(digitalRead(pinoIVReflexivo3) == LOW){
      digitalWrite(pinoLedToIV3, HIGH);
      payloadJson["level"] = 3;
  }else{ 
    digitalWrite(pinoLedToIV3, LOW);
  }

  delay(3000);
}
