   # AsyncClientjj
   - **Library for ESP8266 and ESP32 that manages Asyncronous GET requests to any server (IFTTT,etc)**
  
  [![GitHub version](https://img.shields.io/github/release/jjsanma1/AsynClientjj-Library-for-ESP8266---ESP32.svg)](https://github.com/jjsanma1/AsynClientjj-Library-for-ESP8266---ESP32/releases/latest)
[![GitHub download](https://img.shields.io/github/downloads/jjsanma1/AsynClientjj-Library-for-ESP8266---ESP32/total.svg)](https://github.com/jjsanma1/AsynClientjj-Library-for-ESP8266---ESP32/releases/latest)
[![GitHub stars](https://img.shields.io/github/stars/jjsanma1/AsynClientjj-Library-for-ESP8266---ESP32.svg)](https://github.com/jjsanma1/AsynClientjj-Library-for-ESP8266---ESP32/stargazers)

  It can send requests in paralell to several servers , then if you need the response you can get it asyncronously.
  The maximum simultaneous Requests by default is 5 but it can be modified in the AsyncClientjj.h code of this class. 

  - ## V1.0 . Author: JJ - August 2023. Tested in ESP8266 and ESP32.Arduino IDE 2.1.1. ESP8266 boards 3.1.2 . ESP32 boards 2.0.11
  For ESP8266 it Uses ESPAsyncTCP :https://github.com/dvarrel/ESPAsyncTCP (V1.1.4. date  2023 august 2)
  For ESP32 it Uses   AsyncTCP :https://github.com/dvarrel/AsyncTCP  (V1.2.4. date  2019 )

  - This project contains source code for Visual Studio Code Platform but you can easely convert to Arduino IDE bu renaming main.cpp as main.ino
  
 - As main code you will find two files : main.cpp and advanced.txt .
   main.cpp is the simple example of use of AsyncClientjj class. if you want to use the advanced example you should rename main.cpp as simple.txt and rename advanced.txt 
  to main.cpp

  Basic Code:
  ```c
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#pragma message "Compiling for ESP8266 Device"
#elif defined(ESP32)
#include <WiFi.h>
#pragma message "Compiling for ESP32 Device"
#else
#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif


#include "AsyncClientjj.h"


int step=0;

AsyncClientjj cliente;


void setup() {
#if defined(ESP8266)
  Serial.begin(74880);

//#pragma message "ESP8266 stuff happening!"
#elif defined(ESP32)
  Serial.begin(115200);
#endif
	
  delay(200);  
	// connects to access point
	WiFi.mode(WIFI_STA);
	WiFi.begin("MiFibra-7B0B","byrRt3we");//wifi SSID and Password
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(500);
	}
 Serial.println();
 Serial.println("********** connected to wifi *****************************");
 
}
void loop() {
      cliente.run();// manager that is always running .It checks pending requests and sends them
      int request_Nb=0;
     if(step==0){
        step=1;                  
         String message="GET /";   //server to ask for your own IP  
         request_Nb=cliente.send("api.myip.com",80,&message,true);
         Serial.print("request sent with number: ");Serial.println(request_Nb);// test bad request            
        }
     if(step==1){
        int status_request=cliente.getStatus(request_Nb);
         if(status_request>3){ // if response or error is received
             Serial.print("Request status: ");Serial.println(cliente.getStatusMessage(status_request));// 
            step=2; //end
            String response=cliente.getResponse(0); //gets the response
            Serial.print("response received: ");Serial.println(response);// 
         }
     }        
}
```
