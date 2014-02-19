// ----------------------------------------------------------------------
// Copyright (c) 2014 Pablo Caro. All Rights Reserved.
// Pablo Caro <me@pcaro.es> - https://pcaro.es/
// gamepad.c
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

#include "gamepad.h"

uint8_t BUTTON = 0;
uint8_t JOYPAD = 0;
uint8_t VIRTUAL = 0;

void gamepad_init()
{
    // Configure B port directions
    DDRB |= CLOCK;  // Use CLOCK bit as output
    DDRB |= LATCH;  // Use LATCH bit as output
    DDRB &= ~DATA;  // Use DATA bit as input

    // Configure B port output/input
    PORTB |= CLOCK;   // Set CLOCK bit to 0
    PORTB &= ~LATCH;  // Set LATCH bit to 0
    PORTB &= ~DATA;   // Set DATA bit to normal input
}

void gamepad_read()
{
    cycle_latch();

    // Physical buttons
    BUTTON  = button_read(BUTTON_B);
    BUTTON |= button_read(BUTTON_Y);
    BUTTON |= button_read(BUTTON_SELECT);
    BUTTON |= button_read(BUTTON_START);

    JOYPAD  = button_read(JOYPAD_UP);
    JOYPAD |= button_read(JOYPAD_DOWN);
    JOYPAD |= button_read(JOYPAD_LEFT);
    JOYPAD |= button_read(JOYPAD_RIGHT);

    BUTTON |= button_read(BUTTON_A);
    BUTTON |= button_read(BUTTON_X);
    BUTTON |= button_read(BUTTON_L);
    BUTTON |= button_read(BUTTON_R);

    // Virtual buttons
    // Reboot: Select + Start + L + R

    VIRTUAL = 0;

    if (PRESSED_SELECT) {
        if (PRESSED_START) {
            if (PRESSED_L && PRESSED_R) {
                VIRTUAL = VIRTUAL_REBOOT;

                BUTTON &= ~BUTTON_L;
                BUTTON &= ~BUTTON_R;

                BUTTON &= ~BUTTON_START;
                BUTTON &= ~BUTTON_SELECT;
            }
        }
    }

}

uint8_t button_read(uint8_t button)
{
    uint8_t pressed = (PINB & DATA) ? 0 : button;
    cycle_clock();
    return pressed;
}

void cycle_latch()
{
    // Set LATCH bit to 1
    PORTB |= LATCH;
    _delay_us(12);

    // Set LATCH bit to 0
    PORTB &= ~LATCH;
    _delay_us(6);
}

void cycle_clock()
{
    // Set CLOCK bit to 0
    PORTB &= ~CLOCK;
    _delay_us(6);

    // Set CLOCK bit to 1
    PORTB |= CLOCK;
    _delay_us(6);
}
