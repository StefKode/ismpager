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

#include "mydisplay.h"

/**
 * @brief Construct a new My Display:: My Display object
 *
 * Prints the application name and version on start
 */
MyDisplay::MyDisplay(void)
{
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  if (!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (1)
      ;
  }
  set_text_small("Pager " APPVERSION);
}
/**
 * @brief Clears diplay and writes text with small font
 *
 * @param s Text
 */
void MyDisplay::set_text_small(char *s)
{
  display->setFont();
  display->clearDisplay();
  display->setTextColor(SSD1306_WHITE);
  display->setTextSize(2);
  display->setCursor(0, 10);
  display->println(s);
  display->display();
}

/**
 * @brief Clears diplay and writes text with large font
 *
 * @param s Text
 */
void MyDisplay::set_text_big(char *s)
{
  display->setFont(&FreeSans18pt7b);
  display->clearDisplay();
  display->setTextColor(SSD1306_WHITE);
  display->setTextSize(1);
  display->setCursor(0, 28);
  display->println(s);
  display->display();
}

/**
 * @brief Clears diplay and writes text with large font
 *
 * @param s Text
 */
void MyDisplay::set_text_big(const char *s)
{
  set_text_big((char *)s);
}

/**
 * @brief Clears diplay and writes text with small font font
 *
 * @param s Text
 */
void MyDisplay::set_text_small(const char *s)
{
  set_text_small((char *)s);
}
/**
 * @brief paints the battery symbol with charge level
 *
 * does not clear the display
 *
 * @param level charge level 0..1
 */
void MyDisplay::update_bat_gauge(float level)
{
  unsigned int width = (unsigned int)(level * 14.0);
  display->fillRect(SCREEN_WIDTH - 15, 0, 14, 5, SSD1306_BLACK);
  display->drawRect(SCREEN_WIDTH - 15, 0, 14, 5, SSD1306_WHITE);
  display->fillRect(SCREEN_WIDTH - 15, 0, width, 5, SSD1306_WHITE);
  display->display();
}
