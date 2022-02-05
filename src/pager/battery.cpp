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

#include "Arduino.h"
#include "battery.h"

float get_bat_level()
{
  long vcc;
  unsigned int width;
  float clip;
  const float batbase = 3300.0;
  const float battop  = 4300.0;

  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  vcc = ADCL;
  vcc |= ADCH<<8;
  vcc = 1126400L / vcc; // Back-calculate AVcc in mV
    
  if ((float)vcc > battop) {
    return 1.0;
  } 

  clip = (float)vcc - batbase;
  if (clip < 0.0) {
    return 0.0;
  }
  return clip / (battop - batbase);
}