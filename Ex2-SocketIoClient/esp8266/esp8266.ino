#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include "SocketIoClient.h"

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;

void event(const char * payload, size_t length) {
  Serial.printf("got message: %s\n", payload);
}

void setup() {
    Serial.begin(9600);
    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

      for(uint8_t t = 4; t > 0; t--) {
          Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
          Serial.flush();
          delay(1000);
      }

    WiFiMulti.addAP("WIFI", "qwerty___");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.begin("192.168.0.105", 3484);
    webSocket.on("event", event);
    
    //use HTTP Basic Authorization this is optional remove if not needed
    //webSocket.setAuthorization("username", "password");
}

void loop() {
    delay(2000);
    webSocket.emit("event", "hello " + millis());
    webSocket.loop();
}
