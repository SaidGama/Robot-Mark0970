#include <WebSocketsClient.h>
#include <WiFi.h>
#include "CamDriver.hpp"

#define FLASH      4

WebSocketsClient ws1;
WebSocketsClient ws2;

bool connected = false;
bool flag = false;

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

void ws1Event(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("\nSocket 1 desconectado");
            break;
        case WStype_CONNECTED:
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
            Serial.printf("\nSocket 2 desconectado");
            break;
        case WStype_CONNECTED:
            Serial.println("\nSocket 2 conectado");
            break;
        case WStype_TEXT:{
              recv_message(payload);
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
void recv_message(uint8_t * payload)
{
  ws2.loop();
  String message;
  message = String((char *)payload);
  if(message == "prender"){
        prenderLed(1);
  }else if(message == "apagar") {
        prenderLed(0);
  }else if(message == "camaraon"){
        connected=true;
  }else if(message == "camaraoff"){
        connected=false;
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
  btStop();
  setCpuFrequencyMhz(80);
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

  //ws1.beginSSL("mark0970-server.herokuapp.com", 443, "/socket1");
  //ws2.beginSSL("mark0970-server.herokuapp.com", 443,"/socket2");

  //ws1.begin("192.168.100.9", 3000, "/socket1");
  //ws2.begin("192.168.100.9", 3000, "/socket2");

  ws1.begin("34.125.55.148", 3000, "/socket1");
  ws2.begin("34.125.55.148", 3000, "/socket2");
  
  ws1.onEvent(ws1Event);
  ws2.onEvent(ws2Event);

  ws1.setReconnectInterval(500);
  ws2.setReconnectInterval(500);
}

void loop() {
  ws1.loop();
  ws2.loop();
  if(connected == true){
    liveCam();
  }
  if( (millis()-lastTime) > timerDelay ){
    ws1.sendPing();
    ws2.sendPing();
    lastTime = millis();
  }
}