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

#define APPVERSION "5"
//#define DEBUG

#include <Arduino.h>
#define RH_ENABLE_ENCRYPTION_MODULE
#include <RH_ASK.h>
#include <SPI.h>
#include <Wire.h>
#include <AESLib.h>
#include <LowPower.h>

#include "projectconfig.h"
#include "pagerio.h"
#include "mydisplay.h"
#include "battery.h"
#include "alarm.h"

#ifdef DEBUG
#define dbg_print(S) \
  Serial.println(S); \
  delay(500);
#else
#define dbg_print(S)
#endif

#define sec_micros(s) (s * 1000000)

union datablock_t rxblock;
RH_ASK rf_ask(500);

MyDisplay *disp;
Alarm *alarm;

uint8_t key[] = AESKEY;
const int WAIT_UNTIL_TIME_UPDATE = 0;
const int PROBING_FOR_ALERT_SLEEP_PHASE = 1;
const int PROBING_FOR_ALERT_PRESENT = 2;
const int START_PROBING = 3;
const int START_COLLECTING = 4;
unsigned long int last_time_update;
unsigned long int bat_track;
int alert_probe_counter = 0;
int pager_state;

void low_power_protect()
{
  disp->set_text_small("Low Bat");
  delay(3000);
  digitalWrite(6, LOW);
  digitalWrite(8, LOW);
  while (1)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}

void powersave(int maxcycles)
{
  digitalWrite(6, LOW);
  for (int i = 0; i < maxcycles; i++)
  {
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  }
  digitalWrite(6, HIGH);
}

int check_and_handle_msg()
{
  char d[6];
  uint8_t rxlen;
  int ret = 0;

  memset((void *)&rxblock, 0, sizeof(rxblock));

  if (rf_ask.recv((uint8_t *)&rxblock, &rxlen))
  {
    // rf_ask.printBuffer("RX Raw:", , rxlen);
    if (rxlen != 16)
    {
      dbg_print("wrong data len");
      dbg_print(rxlen);
      return 0;
    }

    aes128_dec_single(key, (uint8_t *)&rxblock);
    // rf_ask.printBuffer("RX Dec:", (uint8_t*)&rxblock, 16);

    dbg_print(rxblock.elem.alarm);
    ret = 1;
    if (rxblock.elem.alarm != 0)
    {
      alarm->evaluate(rxblock.elem.alarm, rxblock.elem.alarm_floor, rxblock.elem.alarm_room);
      ret = 2;
    }

    d[0] = rxblock.elem.h0;
    d[1] = rxblock.elem.h1;
    d[2] = ':';
    d[3] = rxblock.elem.m0;
    d[4] = rxblock.elem.m1;
    d[5] = 0;
    disp->set_text_big(d);
  }
  return ret;
}

void handle_vcc()
{
  float width;
  width = get_bat_level();
  if (width < 0.01)
  {
    low_power_protect();
    // never returns
  }
  disp->update_bat_gauge(width);
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif

  dbg_print("\nRF Receiver");
  last_time_update = micros();
  bat_track = last_time_update;

  pinMode(5, INPUT);  /* Key */
  pinMode(7, OUTPUT); /* BUZZER */
  pinMode(6, OUTPUT); /* RF Power */
  pinMode(8, OUTPUT); /* OLED Power */
  digitalWrite(6, HIGH);
  digitalWrite(8, HIGH);

  disp = new MyDisplay();
  alarm = new Alarm(*disp);
  handle_vcc();

  if (!rf_ask.init())
  {
    dbg_print("ERROR - rf_ask failed");
  }
  else
  {
    dbg_print("RHASK ok");
  }

  pager_state = WAIT_UNTIL_TIME_UPDATE;
}

void loop()
{
  int ret;

  switch (pager_state)
  {
  /* ############################################ */
  case WAIT_UNTIL_TIME_UPDATE:
    if (check_and_handle_msg() == 1)
    {
      pager_state = START_PROBING;
      last_time_update = micros();
      dbg_print("\nSTART_PROBING");
    }
    break;

  /* ############################################ */
  case START_PROBING:
    pager_state = PROBING_FOR_ALERT_SLEEP_PHASE;
    alert_probe_counter = 6;
    dbg_print("PROBING_FOR_ALERT_SLEEP_PHASE");
    break;

  /* ############################################ */
  case PROBING_FOR_ALERT_SLEEP_PHASE:
    powersave(1);

    if (alert_probe_counter == 0)
    {
      pager_state = WAIT_UNTIL_TIME_UPDATE;
      dbg_print("WAIT_UNTIL_TIME_UPDATE");
      break;
    }

    pager_state = START_COLLECTING;
    dbg_print("START_COLLECTING");
    break;

  /* ############################################ */
  case START_COLLECTING:
    delay(4000);
    pager_state = PROBING_FOR_ALERT_PRESENT;
    dbg_print("PROBING_FOR_ALERT_PRESENT");
    break;

  /* ############################################ */
  case PROBING_FOR_ALERT_PRESENT:
    ret = check_and_handle_msg();
    if (ret == 1)
    {
      pager_state = START_PROBING;
      dbg_print("missed sync, START_PROBING");
      break;
    }
    if (ret == 2)
    {
      dbg_print("got alert");
    }

    alert_probe_counter--;
    pager_state = PROBING_FOR_ALERT_SLEEP_PHASE;
    dbg_print("PROBING_FOR_ALERT_SLEEP_PHASE");
    break;
  }

  unsigned long int now = micros();
  if ((now - bat_track) > sec_micros(5))
  {
    if ((now - last_time_update) > sec_micros(130))
    {
      disp->set_text_small("no signal");
    }

    bat_track = micros();
    handle_vcc();
  }
}
