// ----------------------------------------------------------------------
// Copyright (c) 2014 Pablo Caro. All Rights Reserved.
// Pablo Caro <me@pcaro.es> - https://pcaro.es/
// SNUSB.h
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------

#ifndef __SNUSB_H__
#define __SNUSB_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"
#include "gamepad.h"

void reboot(void);
void reset_keys(void);
void press_key(uint8_t, uint8_t);
void press_modifier(uint8_t);
int main(void);

#endif
