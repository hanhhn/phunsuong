/*
 * WebSocketClientSocketIO.ino
 *
 *  Created on: 06.06.2016
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

int LED = 16; // led connected to D0
StaticJsonBuffer<200> jsonBuffer;

const String getEventName(const String msg) {
  return msg.substring(4, msg.indexOf("\"", 4));
}

const String getEventPayload(const String msg) {
  String result = msg.substring(msg.indexOf("\"", 4) + 2, msg.length() - 1);
  if(result.startsWith("\"")) {
    result.remove(0,1);
  }
  if(result.endsWith("\"")) {
    result.remove(result.length()-1);
  }
  return result;
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  String msg;
  
  switch(type) {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        isConnected = false;
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
        isConnected = true;

        // send message to server when Connected
        // socket.io upgrade confirmation message (required)
        webSocket.sendTXT("5");
        break;
    case WStype_TEXT:
        Serial.printf("[WSc] get text: %s\n", payload);
        msg = String((char*)payload);


        if(msg.startsWith("42")) {
          JsonObject& root = jsonBuffer.parseObject(getEventPayload(msg).c_str());
          Serial.printf("Event: %s\n", getEventPayload(msg).c_str());
          
          if(!root.success()) {
            Serial.println("parsing json failed");
            break;
          }

          if(strcmp(getEventName(msg).c_str(), "led") == 0){
            const char* status = root["status"];
            
            if(strcmp(status,"on") == 0) {
              digitalWrite(LED, HIGH); // Turn ON LED
              Serial.printf("Led: %s\n", status);
              break;
            }
            if(strcmp(status,"off") == 0) {
              digitalWrite(LED, LOW); // Turn OFF LED
              Serial.printf("Led: %s\n", status);
              break;
            }
          }
        }
        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);
        // webSocket.sendBIN(payload, length);
        break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  
  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }

  WiFiMulti.addAP("WIFI", "qwerty___");

  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
  }

  webSocket.beginSocketIO("192.168.0.105", 3484);
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  if(isConnected) {
    uint64_t now = millis();

    if(now - messageTimestamp > MESSAGE_INTERVAL) {
      messageTimestamp = now;
      delay(5000);
      webSocket.sendTXT("42[\"repeat\",{\"hello\":\"hello client\"}]");
    }
    
    if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
      heartbeatTimestamp = now;
      webSocket.sendTXT("2");
    }
  }
}
