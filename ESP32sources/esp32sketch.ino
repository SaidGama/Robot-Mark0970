#include <WebSocketsClient.h>
#include <WiFi.h>
#include "CamDriver.h"

#define FLASH      4

WebSocketsClient ws1;
WebSocketsClient ws2;

bool connected = false;

void ws1Event(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("\nDisconnected");
            break;
        case WStype_CONNECTED:
            connected = true;
            Serial.println("\nSocket 1 conectado");
            break;
        case WStype_TEXT:
        case WStype_BIN:
        case WStype_ERROR:      
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}
void ws2Event(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("\nDisconnected");
            break;
        case WStype_CONNECTED:
            connected = true;
            Serial.println("\nSocket 2 conectado");
            break;
        case WStype_TEXT:{
              char * sptr = NULL;
              int mensaje = strtol((char *)payload, &sptr, 10);
              if(strcmp(sptr,"prender") == 0){
                prenderLed(1);
              }else if(strcmp(sptr,"apagar") == 0){
                prenderLed(0);
              }else if(strcmp(sptr,"camaraon") == 0){
                connected=true;
              }else if(strcmp(sptr,"camaraoff") == 0){
                connected=false;
              }
            break;  
        }  
        case WStype_BIN:
        case WStype_ERROR:      
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}

void liveCam(){
  //captura un frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
      Serial.println("Frame buffer could not be acquired");
      return;
  }
  //manda el frame mediante socket1
  ws1.sendBIN(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void prenderLed(int valor){
  if(valor==1){
    digitalWrite(FLASH, HIGH);
  }else if(valor==0){
    digitalWrite(FLASH, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(FLASH,OUTPUT);
  configCamera();
  WiFi.begin("Totalplay-F39E", "F39E15E7MBuWtB5A");
  //WiFi.begin("Pixel3", "12345678");
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado");
  String IP = WiFi.localIP().toString();
  Serial.print("IP address: " + IP);
  //webSocket.begin("34.125.157.251", 3000, "/");
  ws1.begin("192.168.100.9", 3000, "/socket1");
  ws1.onEvent(ws1Event);
  //ws2.begin("34.125.157.251", 3000, "/");
  ws2.begin("192.168.100.9", 3000, "/socket2");
  ws2.onEvent(ws2Event);
}

void loop() {
  ws1.loop();
  ws2.loop();
  if(connected == true){
    liveCam();
  }
}