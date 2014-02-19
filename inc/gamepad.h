// ----------------------------------------------------------------------
// Copyright (c) 2014 Pablo Caro. All Rights Reserved.
// Pablo Caro <me@pcaro.es> - https://pcaro.es/
// gamepad.h
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


#ifndef __GAMEPAD_H__
#define __GAMEPAD_H__

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>


// Button values
#define BUTTON_A        (1 << 0)
#define BUTTON_B        (1 << 1)
#define BUTTON_X        (1 << 2)
#define BUTTON_Y        (1 << 3)
#define BUTTON_L        (1 << 4)
#define BUTTON_R        (1 << 5)
#define BUTTON_SELECT   (1 << 6)
#define BUTTON_START    (1 << 7)

// Pad values
#define JOYPAD_UP       (1 << 0)
#define JOYPAD_DOWN     (1 << 1)
#define JOYPAD_LEFT     (1 << 2)
#define JOYPAD_RIGHT    (1 << 3)

// Virtual values
#define VIRTUAL_REBOOT   (1 << 0)


// Pressed buttons
#define PRESSED_A         (BUTTON & BUTTON_A)
#define PRESSED_B         (BUTTON & BUTTON_B)
#define PRESSED_X         (BUTTON & BUTTON_X)
#define PRESSED_Y         (BUTTON & BUTTON_Y)
#define PRESSED_L         (BUTTON & BUTTON_L)
#define PRESSED_R         (BUTTON & BUTTON_R)
#define PRESSED_START     (BUTTON & BUTTON_START)
#define PRESSED_SELECT    (BUTTON & BUTTON_SELECT)

// Pressed pad directions
#define PRESSED_UP        (JOYPAD & JOYPAD_UP)
#define PRESSED_DOWN      (JOYPAD & JOYPAD_DOWN)
#define PRESSED_LEFT      (JOYPAD & JOYPAD_LEFT)
#define PRESSED_RIGHT     (JOYPAD & JOYPAD_RIGHT)

// Pressed virtual buttons
#define PRESSED_REBOOT    (VIRTUAL & VIRTUAL_REBOOT)


// SNES connector interface
#define CLOCK   (1 << 0)
#define LATCH   (1 << 1)
#define DATA    (1 << 2)


extern uint8_t BUTTON;
extern uint8_t JOYPAD;
extern uint8_t VIRTUAL;

void gamepad_init(void);
void gamepad_read(void);
uint8_t button_read(uint8_t button);
void cycle_latch(void);
void cycle_clock(void);

#endif
