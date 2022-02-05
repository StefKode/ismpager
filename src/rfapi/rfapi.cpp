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
#include <SoftwareSerial.h>
#include <RH_ASK.h> /* needed by RH_ASK */
#include <SPI.h> 
#include <AESLib.h>

#include "projectconfig.h"
#include "pagerio.h"
 
RH_ASK rf_driver(500);

uint8_t key[] = AESKEY;
uint8_t cycle = 0;

// ######################## RFAPI reps ################################
const int rfapi_txpin = 6;
const int rfapi_rxpin = 7;
SoftwareSerial rfapi(rfapi_rxpin, rfapi_txpin);

// ######################## RF Handlers ##############################
void rfrecv(String value)
{
  Serial.println("> send to rf: " + value);
  
  Serial.println("< done");
}


// ########################## MAIN SETUP ####################################
void setup() {
  // Serial Console
  Serial.begin(115200);
  Serial.println("RFAPI");
  
  // Initialize ASK Object
  rf_driver.init();
  
  // Serial interface to rfapi module
  rfapi.begin(9600);
  Serial.println("Software Serial initialized"); 
}

String data;
// ########################## Work Loop ####################################
void loop() 
{
  int len;
  
  if (rfapi.available() > 0) {
    data = rfapi.readString();
    data.trim();
    data.trim();
    const char *out = data.c_str();
    len = strlen(out);

    union datablock_t sendblock;
    memset((void*)&sendblock, 0, sizeof(sendblock));

    /* time */
    Serial.println("Rfapi Rx: " + String(out));
    sendblock.elem.h0 = out[0];
    sendblock.elem.h1 = out[1];
    sendblock.elem.m0 = out[2];
    sendblock.elem.m1 = out[3];
    if (len >= 5) {
      sendblock.elem.alarm = out[4] - '0';
    }
    if (len >= 7) {
      sendblock.elem.alarm_floor = out[5];
      sendblock.elem.alarm_room = out[6];
    }
    sendblock.elem.cycle = cycle;
    cycle += 1;
    
    rf_driver.printBuffer("SendRaw:", (uint8_t*)&sendblock, sizeof(sendblock));
    aes128_enc_single(key, &sendblock);
    rf_driver.printBuffer("SendEnc:", (uint8_t*)&sendblock, sizeof(sendblock));
    rf_driver.send((uint8_t*)&sendblock, sizeof(sendblock));
    rf_driver.waitPacketSent();
    delay(1000);
    rfapi.println("ok\n");
  }
  delay(1000);
}
