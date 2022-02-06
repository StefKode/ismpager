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
#include "alarm.h"

Alarm::Alarm(MyDisplay &dispref) : display(dispref)
{
}

void Alarm::evaluate(uint8_t type, uint8_t flr, uint8_t room)
{
    char *alarmstr;
    int freq, i;

    if (type > 4)
    {
        return;
    }

    if (type == 4)
    {
        alarmstr = display_string("SECU --");
        alarmstr[5] = flr;
        alarmstr[6] = room;
        freq = 1800;
    }
    else if (type == 3)
    {
        alarmstr = display_string("FIRE --");
        alarmstr[5] = flr;
        alarmstr[6] = room;
        freq = 2000;
    }
    else if (type == 2)
    {
        alarmstr = display_string("DOOR BELL");
    }
    else
    {
        alarmstr = display_string("ALARM -");
        alarmstr[6] = '0' + type;
        freq = 1000;
    }

    display.set_text_big(alarmstr);

    if (type != 2)
    {
        while (digitalRead(6) != LOW)
        {
            digitalWrite(6, LOW);
            tone(7, freq);
            delay(400);
            noTone(7);
            digitalWrite(6, HIGH);
            delay(600);
        }
    }
    else
    {
        while (1)
        {
            tone(7, 1900);
            delay(100);
            noTone(7);
            if (digitalRead(5) == LOW)
            {
                return;
            }
            for (i = 0; i < 15; i++)
            {
                tone(7, 1500);
                delay(60);
                noTone(7);
                if (digitalRead(5) == LOW)
                {
                    return;
                }
                delay(60);
                if (digitalRead(5) == LOW)
                {
                    return;
                }
            }
            for (i = 0; i < 70; i++)
            {
                delay(100);
                if (digitalRead(5) == LOW)
                {
                    return;
                }
            }
        }
    }
}
