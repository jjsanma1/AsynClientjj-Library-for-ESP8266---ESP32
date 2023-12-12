/*
 * JJ - June 2023. Tested in ESP8266
  Example of how to use of AsyncClientjj class- This Library manages Async HTTP requests to any server (IFTTT,etc)
  it can send several queries in paralell to different API servers to retrieve information in asyncronous mode. 
*/
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
