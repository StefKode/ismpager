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

#ifndef MYDISPLAY_H
#define MYDISPLAY_H
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans18pt7b.h>

#ifndef APPVERSION
#define APPVERSION "-"
#endif

#define display_string(s) (char *)s

/**
 * @brief Class to manage OLED display
 *
 */
class MyDisplay
{
  Adafruit_SSD1306 *display;

public:
  MyDisplay(void);
  /**
   * @brief Clears diplay and writes text with small font
   *
   * @param s Text
   */
  void set_text_small(char *s);

  /**
   * @brief Clears diplay and writes text with small font
   *
   * @param s Text
   */
  void set_text_small(const char *s);

  /**
   * @brief Clears diplay and writes text with large font
   *
   * @param s Text
   */
  void set_text_big(char *s);

  /**
   * @brief Clears diplay and writes text with large font
   *
   * @param s Text
   */
  void set_text_big(const char *s);

  /**
   * @brief paints the battery symbol with charge level
   *
   * @param level charge level 0..1
   */
  void update_bat_gauge(float level);
};

#endif