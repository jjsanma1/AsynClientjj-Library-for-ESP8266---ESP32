   # AsyncClientjj
   - **Is a Library for ESP8266 and ESP32 that manages Asyncronous GET requests to any server (IFTTT,etc)**
  It can send requests in paralell to several servers , then if you need the response you can get asyncronously.
  The maximum simultaneous Requests by default is 5 but it can be modified in the code of this class. 

  - V1.0 . Author: JJ - August 2023. Tested in ESP8266 and ESP32.Arduino IDE 2.1.1. ESP8266 boards 3.1.2 . ESP32 boards 2.0.11
  For ESP8266 it Uses ESPAsyncTCP :https://github.com/dvarrel/ESPAsyncTCP (V1.1.4. date  2023 august 2)
  For ESP32 it Uses   AsyncTCP :https://github.com/dvarrel/AsyncTCP  (V1.2.4. date  2019 )

  - This project contains source code for Visual Studio Code Platform but you can easely convert to Arduino IDE bu renaming main.cpp as main.ino
  
 - As main code you will find two files : main.cpp and advanced.txt .
   main.cpp is the simple example of use of AsyncClientjj class. if you want to use the advanced example you should rename main.cpp as simple.txt and rename advanced.txt 
  to main.cpp
