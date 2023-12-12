/*
 * JJ - June 2023. Tested in ESP8266.
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



long unsigned oldmillis=0;
long unsigned oldmillis1=0;
long unsigned oldmillis2=0;
long count=0;
int status=0;

AsyncClientjj cliente;


void setup() {
#if defined(ESP8266)
  Serial.begin(74880);

//#pragma message "ESP8266 stuff happening!"
#elif defined(ESP32)
  Serial.begin(115200);
#endif
	
	delay(200);
  Serial.print("getFreeHeap: ");Serial.println(ESP.getFreeHeap());
  #if defined(ESP8266)
    Serial.print("  heap fragm: ");Serial.println(ESP.getHeapFragmentation());
  #endif
	// connects to access point
	WiFi.mode(WIFI_STA);
	WiFi.begin("SSID","PASSWORD");//Use your own wifi SSID and Password
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(500);
	}
 Serial.println();
 Serial.println("********** connected to wifi *****************************"); 
 
char data='\0';
while (!data){
  if (Serial.available()){
      data = Serial.read();
  }
}
 Serial.println(data);
   
}
/* Uses AsyncClientjj to send get requests to several servers periodically. It shows the response of the AsyncClientjj object in
the terminal*/
void loop() {
     cliente.run();// manager that is always running .It checks pending requests and sends them
     if(status==0){
        if(millis()>oldmillis+5000){    
              oldmillis=millis(); // sends requests to the manager client every x millis 
              status=1;  
               
              // get requests to several servers . Use your own keys and passwords
              String message;
              //IFTTT example
              message="GET /trigger/V1/with/key/yourkey";             
              Serial.print("pending request: ");Serial.println(cliente.send("maker.ifttt.com",80,&message,true));// test request ok              
              //openweathermap example              
              message="GET /data/2.5/forecast?q=YOURCITY,ES&APPID=yourID&mode=json&units=metric&cnt=2";             
              Serial.print("pending request: ");Serial.println(cliente.send("api.openweathermap.org",80,&message,true));// test ok           
              //MyIP example 1
              message="GET /";       
              Serial.print("pending request: ");Serial.println(cliente.send("api.myip.com",80,&message,true));// test request ok              
              //MyIP example 2 just to test error 1
              message="GET /api.ipify.org";       
              Serial.print("pending request: ");Serial.println(cliente.send("error",80,&message,false));// test connection not possible
              //MyIP example 3 just to test error 2
              message="GET /api.ipify.org";  
              Serial.print("pending request: ");Serial.println(cliente.send("www.google.com",443,&message,false));// test timeout after connection   

              status=1;      
              oldmillis1=millis(); // sends requests to the manager client every x millis 
      }
    }
      if(status==1){   
            if(millis()>oldmillis1+2000){                 
                Serial.print(" Response 0: ");Serial.println(cliente.getResponse(0));   
                Serial.print(" Response 1: ");Serial.println(cliente.getResponse(1));
                Serial.print(" StatusMessage 0: ");Serial.println(cliente.getStatusMessage(cliente.getStatus(0)));
                Serial.print(" StatusMessage 1: ");Serial.println(cliente.getStatusMessage(cliente.getStatus(1)));
                Serial.print(" StatusMessage 2: ");Serial.println(cliente.getStatusMessage(cliente.getStatus(2)));
                Serial.print(" StatusMessage 3: ");Serial.println(cliente.getStatusMessage(cliente.getStatus(3))); 
                Serial.print(" StatusMessage 4: ");Serial.println(cliente.getStatusMessage(cliente.getStatus(4)));
                oldmillis1=millis(); // sends requests to the manager client every x millis 
                status=0;      
      }     

    }
    
    if(millis()>oldmillis2+200){
      oldmillis2=millis();
      count++;  // just to confirm that main loop is not blocked    
      //Serial.print(count);
    }
}
