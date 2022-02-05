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

#include <stdio.h>
#include <stdint.h>

void encode(uint8_t *key, uint8_t *data)
{
  uint8_t last = 0;
  for (int i=0; i<16; i++) {
    data[i] = (data[i] ^ key[i]) ^ last;
    last = data[i];
  }
}

void decode(uint8_t *key, uint8_t *data)
{
  uint8_t last = 0;
  for (int i=0; i<16; i++) {
    data[i] = (data[i] ^ key[i]) ^ last;
    last = data[i];
  }
}

void printit(char *what, uint8_t *data) {
    printf("%s: ", what);
    for (int i=0; i<16; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

int main() {
    uint8_t key[] =  {0x01, 0x10, 0x53, 0x6f, 0x4c, 0x68, 0xfa, 0xd0, 0xb2, 0x7a, 0xb2, 0x5e, 0x28, 0x44, 0xd1, 0xf9};
    uint8_t data[] = {0x11, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    printit("key         ", key);
    printit("data before ", data);
    encode(key, data);
    printit("data encode ", data);
    decode(key, data);
    printit("data decode ", data);
}
