/*
 *
 * This file is part of the ismpager distribution (https://github.com/StefKode/ismpager).
 * Copyright (c) 2022 Stefan Koch
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#include "projectconfig.h"

const int led = LED_BUILTIN;
ESP8266WebServer server(80);

// ######################## RFAPI reps ################################
SoftwareSerial rfapi;
const int rfapi_txpin = 12;
const int rfapi_rxpin = 2;

// ######################## Wifi Preps ################################
const char* ssid = PROJECT_SSID;
char password[20];
String readLineData;
String response;
void readEEPROM(char addr, char buf[], int len);
void writeEEPROM(char addr, String data);
void getSavePassword();
void readLine();

// ######################## RF Handlers ##############################
String cmd_rfsend(String value)
{
  Serial.println("send-to-rf: " + value);
  rfapi.println(value);
  Serial.println("..done");
  response = rfapi.readString();
  Serial.println("rx-from-rf:");
  Serial.println(response);
  response.trim();
  response.trim();
  return response;
}

// ######################## Web Handlers ##############################
void handleForm() 
{
  String reply = "error, timeout";
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    for (uint8_t i = 0; i < server.args(); i++) {      
      if (server.argName(i) == "rfsend") {
        Serial.println("rfsend->");
        while (rfapi.available() <= 0) {}
        reply = cmd_rfsend(server.arg(i));
      }
    }
    // test response delay
    //delay(2000);
    server.send(200, "text/plain", reply);
  }
}

void handleNotFound() 
{
  server.send(404, "text/plain", "not found");
}

// ########################## MAIN SETUP ####################################
void setup(void) 
{
  pinMode(led, OUTPUT);
  pinMode(rfapi_rxpin, INPUT);
  digitalWrite(led, LOW);

  // Configuration Data
  EEPROM.begin(512);
  
  // Serial Console
  Serial.begin(115200);
  Serial.println("WEBAPI");
  
  // Serial interface to rfapi module
  rfapi.begin(9600, SWSERIAL_8N1, rfapi_rxpin, rfapi_txpin, false);
  if (!rfapi) {
    Serial.println("Invalid SoftwareSerial pin configuration, check config"); 
    // Stop here
    while (1) {
      delay (1000);
    }
  }
  Serial.println("Software Serial initialized"); 

  //######################## WIFI ########################################

  /* ------------------------------------
   * read wifi password and start wifi
   */
  readEEPROM(0, password, sizeof(password));

  // Connect to WiFi network
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //Assign static IP by fritz.box
  //WiFi.mode(WIFI_STA);
  //WiFi.config(ip, gateway, subnet);

  // start wifi
  WiFi.begin(ssid, password);

  int maxWait = 20;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");

    maxWait--;
    if (maxWait <= 0) {
      getSavePassword();
      Serial.print("please restart");
      while (1);
    }
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WiFi connected");

  //######################## WebServer ########################################
    
  server.on("/webrfgw-cmd", handleForm);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

//######################## EPROM/Password Utils ###############################
void readLine() {
  while (1) {
    while (Serial.available() <= 0);
    char recieved = Serial.read();
    Serial.println(".");
    readLineData += recieved; 

    // Process message when new line character is recieved
    if (recieved == '\n') {
      return;
    }
  }
}

void getSavePassword()
{
  Serial.print("Enter Password");  
  readLine();
  Serial.print("OK, save to EEPROM");
  
  writeEEPROM(0, readLineData);
  Serial.print("OK, saved");
}

void writeEEPROM(char addr, String data)
{
  int len = data.length();
  int i;
  for(i = 0; i < len; i++) {
    EEPROM.write(addr + i, data[i]);
  }
  
  EEPROM.write(addr + len,'\0');
  EEPROM.commit();
}

void readEEPROM(char addr, char buf[], int len)
{
  int i = 0;
  buf[0] = 0;
  
  for (i = 0; i < len; i++) {
    buf[i] = EEPROM.read(addr + i);
    if (buf[i] == 0) {
      break;
    }
    if (!isPrintable(buf[i])) {
      buf[i] = 0;
      break;
    }
  }
  
  if (i >= len) {
    buf[len - 1] = 0;
  }
  
  return;
}
