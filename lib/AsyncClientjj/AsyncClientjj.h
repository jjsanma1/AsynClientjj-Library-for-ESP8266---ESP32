/*
  AsyncClientjj - Is a Library for ESP8266 and ESP32 that manages Asyncronous GET requests to any server (IFTTT,etc)
  It can send requests in paralell to several servers , then if you need the response you can get asyncronously.
  The maximum simultaneous Requests by default is 5 but it can be modified in the code of this class. 
  V1.0 . Author: JJ - August 2023. Tested in ESP8266 and ESP32.Arduino IDE 2.1.1. ESP8266 boards 3.1.2 . ESP32 boards 2.0.11
  For ESP8266 Uses ESPAsyncTCP :https://github.com/dvarrel/ESPAsyncTCP (V1.1.4. date  2023 august 2)
  For ESP32 Uses   AsyncTCP :https://github.com/dvarrel/AsyncTCP  (V1.2.4. date  2019 )
  
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.  
*/
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h> //https://github.com/dvarrel/ESPAsyncTCP
extern "C" {
#include <osapi.h>
#include <os_type.h>
}
//#pragma message "ESP8266 stuff happening!"
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h> //https://github.com/dvarrel/AsyncTCP
//#pragma message "ESP32 stuff happening!"
#else
//#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif
#ifndef AsyncClientjj_h // to avoid compilation errors if someone calls more than once the library
#define AsyncClientjj_h

#include <UrlEncode.h> // to send messages through whatsapp https://github.com/plageoj/urlencode  https://randomnerdtutorials.com/esp8266-nodemcu-send-messages-whatsapp/ 
#include "config.h"

struct _request{ //structure that contains all the information for every request
    int status=0; // request free
    int localport=0;// output: local port used in this device for the socket. Only valid if the connection is stablished
    char* host; // input : server to connect to
    int port=0;   // input : TCP port of the server to connect to
    String req="";  //input : request to be sent        
    bool reply=false;//true: user wants to receive the reply of the server otherwise , the query is sent but the response from server is not used,
    String response="";    
    int port_old; 
   } ;


// class definition
class AsyncClientjj{
 
  public:// public functions and variables
      AsyncClientjj();// constructor  .filtering time is the delay to ON of the signal    
 
      static const int Max_Requests=5;//Output : Max number of simultaneous requests allowed. they will be serialized and run function will manage one by one.
      int send(const char* host,int port,String* req,bool reply); // puts in the buffer the request to be sent when possible. it returns the number of the request to be used later if you want to know the status , error etc.if no buffer available it returns -1
      void run(); // manages the serialization of the requests to be sent. must be executed in the main loop constantly
      //unsigned long time_out=2000;// input: timeout for connection pending or once it is connected to receive an answer
      int getStatus(int requestnb); //retrieves the status of the request.
      // 1=pending to connect to the server  2=connection request sent & pending of ack, pending of reply 3=connection stablished and request sent , pending of reply 4=reply received ok(HTTP 200)
      //5=reply received with error (HTTP not 200) 6=Error received in the AsyncClient class 7=time-out in the AsyncClient class 8=time-out , reply not received from server on time
      //String getResult(int requestnb);//retrieves the result of the request sent (only valid if status>3)
      String getResponse(int requestnb);//retrieves the text of the response of the server. Max length=1500 characters
      String getStatusMessage(int request_status);//retrieves the result of the request sent (only valid if status>3)
      
  private: // private functions and variables
    AsyncClient _AsyncClient;
    AsyncClient* _client=&_AsyncClient;
      void _onConnect(AsyncClient*);
      void _onDisconnect(AsyncClient* client); 
      void _onData(void* arg, AsyncClient* client, void *data, size_t len);
      void _onPacket(void* arg, AsyncClient* client, struct pbuf *pb); 
      void _onError(AsyncClient* client, int8_t error);
      int getNexRequest();
    _request* request[Max_Requests];
    int p=0;
    int request_in_process=0;
    bool pending=false;
    unsigned long time_out_timer=0;//timer for timeout     
    int count=0;
    int count1=0;
    char* in=NULL;
    int  count_nr(char* c,const char* cc);//c = where to search  cc= what to search    returns number of occurrences
    String extract_str(String* s,const char* cc,int p);
    bool process_response=false;
    int timer_end=0;
    long t_test=0;
    int t_status=0;
    int leng=0;
    int s=0;//status of the client tcp
    bool edge1=false;
    bool edge2=false;
    bool edge3=false;
    bool edge4=false;
    bool edge5=false;    
    void release_request(int n);
    
}; 
#endif
