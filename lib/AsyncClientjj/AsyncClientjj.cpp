/*
  AsyncClientjj - Is a Library for ESP8266 and ESP32 that manages Asyncronous GET requests to any server (IFTTT,etc)
   It can send requests in paralell to several servers , then if you need the response you can get asyncronously. 
   The maximum simultaneous Requests by default is 5 but it can be modified in the code of this class. V1.0 . 
   
   Author: JJ - August 2023. Tested in ESP8266 and ESP32.Arduino IDE 2.1.1. ESP8266 boards 3.1.2 . 
   ESP32 boards 2.0.11 For ESP8266 it Uses ESPAsyncTCP :https://github.com/dvarrel/ESPAsyncTCP (V1.1.4. date 2023 august 2) 
   For ESP32 it Uses AsyncTCP :https://github.com/dvarrel/AsyncTCP (V1.2.4. date 2019 )

  This project contains source code for Visual Studio Code Platform but you can easely convert to Arduino IDE bu renaming main.cpp as main.ino

  As main code you will find two files : main.cpp and advanced.txt . main.cpp is the simple example of use of AsyncClientjj class. if you want to use the advanced example you should rename main.cpp as simple.txt and rename advanced.txt to main.cpp
  
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.  
*/
#include "AsyncClientjj.h"

// constructor
AsyncClientjj::AsyncClientjj(){   
 if (_client) _client->close(true);  // just make sure socket is closed before starting
 //******* Redirect functions from AsyncClient class to functions of this class 
     _client->onConnect([](void *obj, AsyncClient * client) { 
          ((AsyncClientjj*)(obj))->_onConnect(client);
        }, this);
     _client->onDisconnect([](void *obj, AsyncClient * client) { 
          ((AsyncClientjj*)(obj))->_onDisconnect(client);
        }, this);
     _client->onData([](void *obj, AsyncClient* client, void *data, size_t len)  {
     (void) client;
     ((AsyncClientjj*)(obj))->_onData(obj,client,data, len);
     }, this);
     // onPacket crashes some times so do not use
     /* _client->onPacket([](void *obj, AsyncClient* client, struct pbuf *pb)  {
     (void) client;
     ((AsyncClientjj*)(obj))->_onPacket(obj,client,pb);
     }, this);*/
     _client->onError([](void *obj, AsyncClient * client, uint32_t error) {
        ((AsyncClientjj*)(obj))->_onError(client, error);
      }, this);  

}
 //******* CallBack Functions redirected from  AsynClient
void AsyncClientjj::_onConnect(AsyncClient* client) {    
  if(s==1) s++;   //s=2 -> connected
}
void AsyncClientjj::_onDisconnect(AsyncClient* client) {   
if(s!=2 && s!=3 && s!=4) return;// if client was not in the appropiate status to disconnect  
  s=5;//Server get disconnected
  
}
void  AsyncClientjj::_onError(AsyncClient* client, int8_t error){    
   if(s!=1 && s!=2 && s!=3 && s!=4) return;// if client was not in the appropiate status to disconnect
   if(!request[request_in_process]) return;
   if(request[request_in_process]->localport!=client->localPort()) return;// not this connection   
   s=6;
}
void  AsyncClientjj::_onData(void *obj, AsyncClient* client, void *data, size_t len){   
  if(request[request_in_process]){ //If requests exists         
          if (client->localPort()!=request[request_in_process]->localport){// if received local port matches the expected local port
          return;
        }
        if(request[request_in_process]->port_old!=client->localPort()){//detects first packet received for this port
               request[request_in_process]->port_old=client->localPort(); //resets port    
               leng=len;// gets the len of the message to be used later
               if(strstr((char*)data,"200 OK")){//  Reply OK from server
                      t_status=4;                          
               }
               else {//Reply NOT OK from server
                      t_status=5;                             
               }      
                if(request[request_in_process]->reply && leng<1500){// if reply required by user-> put response data in the response of the request object
                        char *a=(char*)data;
                        size_t b=0;
                        while (b<len){                          
                          request[request_in_process]->response+=a[b]; 
                          b++;  
                        }                        
                }                
        }  
        else{//second,third,etc packet received for this port
                 leng+=len;// gets the len of the message to be used later
                 if(request[request_in_process]->reply && leng<1500){// if reply required by user
                   char *a=(char*)data;
                        size_t b=0;
                        while (b<len){                          
                          request[request_in_process]->response+=a[b]; 
                          b++;  
                        }                                 
                 }                                        
        }
  }//If requests exists
s=4;// jump to data reception step      
}

void AsyncClientjj::run(){  
//checks for time-out of TCP connection and jumps to time-out status
if (s>0 && s<5){ 
    if(millis()>time_out_timer+2000){
           time_out_timer=millis();   
            s=7;      
    }
}
// main routine to manage TCP connection to Server. each case is a different step of the process.
  switch(s){
     int n;
          case 0://request in queue, try to send the next request
                n=getNexRequest(); //gets the next request in queue to be sent                      
                   if(n!=-1){ //if request available
                        if (_client->connected()){ //if client is still connected for some reason , then close it
                           _client->close(true);                                                             
                        }                        
                        _client->connect(request[n]->host, request[n]->port);  // connect to the server                                     
                               request[n]->status=2;// request for connection sent and waiting for connection                              
                               request_in_process=n;
                               time_out_timer=millis();                               
                               s=1;//waiting for connection                      
                   } 
          break;
          case 1://waiting for connection
              if(!edge1){
                edge1=true;  
              }  
          break;
          case 2://client connected
          edge1=false;
             if(request[request_in_process]){
                      request[request_in_process]->localport=_client->localPort();//saves the port to identify the reply                      
                      request[request_in_process]->status=3;//status= connected , request sent and waiting for answer
                      request[request_in_process]->req+=" HTTP/1.1\n";
                      request[request_in_process]->req+="Host: ";
                      request[request_in_process]->req+=request[request_in_process]->host;     
                      request[request_in_process]->req+= "\n";  
                      request[request_in_process]->req+="Connection: close\n\n";  
                      char buf[request[request_in_process]->req.length()+1];   
                      request[request_in_process]->req.toCharArray(buf, request[request_in_process]->req.length()+1);
                      _client->add(buf, strlen(buf));
                      if(_client->canSend()) {// tries to send the request after connection to server
                        if(_client->send()){  
                          request[request_in_process]->status=3;//status= connected , request sent and waiting for answer                           
                          leng=0;
                          s=3; 
                        }
                        else{
                           _client->abort();                           
                        }
                      }  
             }                       
          break;     
          case 3://data sent to server. waiting for data reception
               edge4=false;
            
          break;
                     
          case 4://receiving data.wait for disconnection             
               if(!edge4){
                  edge4=true;                  
               }               
              if(_client->disconnected()){ 
                s=5;// server has disconnected (it is also detected in the onDisconnect event) jump to next step for data processing
              }
                        
          break;         
          case 5://server  disconnected -> process data
               edge4=false;      
                 if(request[request_in_process]){// if request has not been deleted because reply=false
                       if (leng==0){// no data sent by server but it disconnected the socket
                              request[request_in_process]->status=9;// status= no answer from server after connection  
                               if (!request[request_in_process]->reply) release_request(request_in_process); //release memory as user doesn't need reply/response
                       }
                       else if (leng>1499) {                        
                             request[request_in_process]->status=10;// // answer too long
                             if (!request[request_in_process]->reply) release_request(request_in_process);  //release memory as user doesn't need reply/response                                                         
                       }
                       else {                                                    
                             if(request[request_in_process]->reply && leng<1500) {
                                  request[request_in_process]->response= extract_str(&request[request_in_process]->response,"\n\r",2);// gets the body of the frame->last string after the separator 
                                  request[request_in_process]->status=t_status;
                            }
                             else{
                               request[request_in_process]->status=4; 
                               release_request(request_in_process); //release memory as user doesn't need reply/response
                            } 
                       }                  
                 }
                 s=0;//jump to the first status             
          break;
          case 6:// error from socket TCP              
               Serial.print("  error from socket ");Serial.println(request_in_process); 
               _client->abort();       
                if(request[request_in_process]){
                  request[request_in_process]->status=6;                           
                 if (!request[request_in_process]->reply) release_request(request_in_process); //release memory as user doesn't need reply/response                                 
                }
               s=0;
               
          break;
           case 7:// time out this detected by the client        
               if(request[request_in_process]){
                 request[request_in_process]->status=7;// status= time-out   
                 if (!request[request_in_process]->reply) release_request(request_in_process);//release memory as user doesn't need reply/response 
               }
                _client->abort();
                s=0;//jump to the first status         
           break;                
          default:        
          break;
  }//end switch
}//end run
// puts the next request that can be put in the queue to be sent to the server
int AsyncClientjj::send(const char* host,int port,String* req,bool reply){  
  // assigns the  request to be sent
  bool found=false;int pp=0;   
  while (pp<Max_Requests){ // finds next free request       
       if(!request[pp]){
        request[pp]=new _request;   // creates a new request as it is required 
       }
       if (request[pp]->status==0 || request[pp]->status>3){    // if request is done  
            found=true;
            release_request(pp);//delete request data     
            request[pp]->status=1;// status= request pending
            request[pp]->host=(char*)host;// remote host to connect to
            request[pp]->port=port;// remote port to connect to
            request[pp]->req=*req;// 
            request[pp]->reply=reply;                      
            //request[pp]->response="";
            //Serial.print(F("  End New Request in queue: "));Serial.print(pp); Serial.print(F(" Status: "));//Serial.println(request[pp]->status); 
       } 
       if(found)return pp;else pp++;            
   }//end while   
  return -1; // no available request
}
// gets the next request in the qeue to be sent in the run function
int AsyncClientjj::getNexRequest(){
  int pp=0;
            while (pp<Max_Requests){ // finds next free request        
             if(request[pp]) {// only if request exists             
                   if (request[pp]->status==1){// only if request is in the queue                   
                      return(pp);
                   }                                             
           }//end if request exists  
           pp++;                      
       }//end while 
  return -1;
}
//function that counts the number of occurrences of a chain in another chain //c = where to search  cc= what to search  returns number of occurrences
int  AsyncClientjj::count_nr(char* s,const char* cc){
if(!s) return 0;
int count=0;
        char* a=s;
        while ((size_t)a<(size_t)s+strlen(s)){
              char* in;
              in=strstr(a,cc);
                 if(in) {                          
                    a=in;              
                    count++;                    
                 }
                 a++;
        } 
return count;  
}
//returns the chain selected by p that is between by the  chain cc//c = where to search  cc= what to search  p=chain in between . returns chain in between defined by p
String AsyncClientjj::extract_str(String* s,const char* cc,int p){
if(p==0) return("");
if(!s) return("");
if(s->length()<strlen(cc)) return("");
if (p<1) return("");
int count=1;  
char* in;
int v=0;
char c[s->length()+1];  
int prev=(int)c;
int last=0;
s->toCharArray(c,s->length()+1);
        char* a=c;
        while ((size_t)a<(size_t)c+strlen(c)){
              in=NULL;
              in=strstr(a,cc);
                 if(in) {
                     v++;                      
                     if(count>=p){
                      last=(int)in;                             
                      break;
                    }                                                                      
                     else {                      
                      prev=(int)in;                     
                      count++;                      
                      a=in;                      
                    }
                 }
                 a++;
        } 
    if(v==0) return("");//chain2 not found in chain 1
    if(p>v+1) return(""); //chain2 not found in chain 1 in the p position selected
    int crg;
    //String m;
    if (p==1) crg=0;
    else crg=strlen(cc)+1;
    if(v==p-1){  
      return (s->substring(prev-(int)c+crg,(int)c+s->length()-1));
    }
    else {
      return (s->substring(prev-(int)c+crg,(last-(int)c)));
    }

}
//gets the status of a request
int AsyncClientjj::getStatus(int requestnb){
  if (!request[requestnb])return(0);//request not in use  
  else if (requestnb>Max_Requests)return(-1);//Max Request is reached
  else {
    return (request[requestnb]->status); 
  }
}
//gets the response of a request if so
String AsyncClientjj::getResponse(int requestnb){
    if (!request[requestnb])return(F(""));//request not in use
    else if (requestnb>Max_Requests)return(F(""));//Max Request is reached
    else if (!request[requestnb]->reply)return(F(""));//User did not select reply for this request  
    else if (request[requestnb]->status<4)return(F(""));//request is still pending
    else {                      
        String m=request[requestnb]->response;                  
        release_request(requestnb);         
        return (m);
    } 
}
//returns the message of a result code
String AsyncClientjj::getStatusMessage(int request_status){
   switch(request_status){
      case-1:return(F("Max Request is reached"));break;
      case 0:return(F("Request not in use"));break;
      case 1:return(F("Request in queue"));break;
      case 2:return(F("Request waiting for TCP connection"));break;
      case 3:return(F("Request connected"));break;
      case 4:return(F("OK.Valid Response from Server"));break;
      case 5:return(F("Error:Bad Response from Server"));break;
      case 6:return(F("Error:undefined error from AsyncClient class"));break;    
      case 7:return(F("Error:Time-Out Server"));break; 
      case 8:return(F("no '\n\r' in the response"));break;
      case 9:return(F("Server disconnected with no data"));break;
      case 10:return(F("Response too long"));break;
      default:return(F("Undefined error"));break; 
   }
}
//releases memory of a request
void AsyncClientjj::release_request(int n){
        request[n]->req="";
        request[n]->response="";
}
