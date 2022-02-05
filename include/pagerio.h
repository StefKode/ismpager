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

#ifndef PAGER_IO
#define PAGER_IO


struct pagerio_t {
  uint16_t cycle;
  char h0;
  char h1;
  char m0;
  char m1;
  uint8_t alarm;
  char alarm_floor;
  char alarm_room;
};

union datablock_t {
  uint8_t maxsize[16];
  struct pagerio_t elem;
};

#endif
