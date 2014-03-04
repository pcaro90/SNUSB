// ----------------------------------------------------------------------
// Copyright (c) 2014 Pablo Caro. All Rights Reserved.
// Pablo Caro <me@pcaro.es> - https://pcaro.es/
// SNUSB.c
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

#include "SNUSB.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_ON		(PORTD |= (1<<6))
#define LED_OFF		(PORTD &= ~(1<<6))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

volatile int ready = 1;

void reboot()
{
    cli();
    UDCON = 1;
    USBCON = (1<<FRZCLK);
    UCSR1B = 0;
    _delay_ms(5);

    EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
    DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
    PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;

    asm volatile("jmp 0x7E00");
}


void reset_keys()
{
    int i;
    for (i = 0; i < 6; ++i) {
        keyboard_keys[i] = 0;
    }

    keyboard_modifier_keys = 0;
}

void press_key(uint8_t key, uint8_t index)
{
    keyboard_keys[index] = key;
}

void press_modifier(uint8_t modifier)
{
    keyboard_modifier_keys |= modifier;
}

int main()
{
	// set for 16 MHz clock
	CPU_PRESCALE(0);

	// Configure all port B and port D pins as inputs with pullup resistors.
	DDRD = 0x00;
	DDRB = 0x00;
	PORTB = 0xFF;
	PORTD = 0xFF;

    // Turn the LED on during the configuration
    LED_CONFIG;
    LED_ON;

	// Initialize the USB, and then wait for the host to set configuration.
	usb_init();
	while (!usb_configured());

    // Initialize the gamepad interface
    gamepad_init();

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

    // Timer 0 configuration (~60Hz)
	TCCR0A = 0x00;  // Normal mode
	TCCR0B = 0x05;  // Clock/1024
	TIMSK0 = (1<<TOIE0);

    LED_OFF;

	while (1) {
        while (!ready);  // Block until the next cycle (~60Hz)
        cli();
        ready = 0;
        sei();

        // Read pressed buttons from gamepad interface
        gamepad_read();

        // Reset key array
        reset_keys();

        // Special functions
        // - Software reboot
        if (VIRTUAL_REBOOT) {
            reboot();
        }

        // 6 keys can be sent at a time, with any number of modifiers.
        //
        // - Buttons A, B, X and Y have their own position in the key array.
        // - Up/down and left/right pairs share one position, as they are
        //     mutually exclusive (you cannot pres up AND down).
        // - L and R use the left and right Shift modifiers.
        // - Select and Start use the left and right Ctrl modifiers.

        if (PRESSED_A) press_key(KEY_Z, 0);
        if (PRESSED_B) press_key(KEY_X, 1);
        if (PRESSED_X) press_key(KEY_A, 2);
        if (PRESSED_Y) press_key(KEY_S, 3);

        if (PRESSED_UP) {
            press_key(KEY_UP, 4);
        } else if (PRESSED_DOWN){
            press_key(KEY_DOWN, 4);
        }

        if (PRESSED_LEFT) {
            press_key(KEY_LEFT, 5);
        } else if (PRESSED_RIGHT){
            press_key(KEY_RIGHT, 5);
        }

        if (PRESSED_L) press_modifier(KEY_LEFT_SHIFT);
        if (PRESSED_R) press_modifier(KEY_RIGHT_SHIFT);
        if (PRESSED_SELECT) press_modifier(KEY_LEFT_CTRL);
        if (PRESSED_START) press_modifier(KEY_RIGHT_CTRL);

        usb_keyboard_send();
	}
}

// This interrupt routine is run approx 61 times per second.
// A very simple inactivity timeout is implemented, where we
// will send a space character.
ISR(TIMER0_OVF_vect)
{
    ready = 1;
}
