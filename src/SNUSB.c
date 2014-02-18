// ---------------------------------------------------
// Copyright (c) 2014 Pablo Caro
// Pablo Caro <me@pcaro.es> - https://pcaro.es/
// SNUSB.c

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_ON		(PORTD &= ~(1<<6))
#define LED_OFF		(PORTD |= (1<<6))
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

uint8_t number_keys[10]=
	{KEY_0,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9};

uint16_t idle_count=0;

int main(void)
{
	uint8_t b, d, mask, i, reset_idle;
	uint8_t b_prev=0xFF, d_prev=0xFF;

	// set for 16 MHz clock
	CPU_PRESCALE(0);

	// Configure all port B and port D pins as inputs with pullup resistors.
	// See the "Using I/O Pins" page for details.
	// http://www.pjrc.com/teensy/pins.html
	DDRD = 0x00;
	DDRB = 0x00;
	PORTB = 0xFF;
	PORTD = 0xFF;

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

	// Configure timer 0 to generate a timer overflow interrupt every
	// 256*1024 clock cycles, or approx 61 Hz when using 16 MHz clock
	// This demonstrates how to use interrupts to implement a simple
	// inactivity timeout.
	TCCR0A = 0x00;
	TCCR0B = 0x05;
	TIMSK0 = (1<<TOIE0);

	while (1) {
		// read all port B and port D pins
		b = PINB;
		d = PIND;
		// check if any pins are low, but were high previously
		mask = 1;
		reset_idle = 0;
		for (i=0; i<8; i++) {
			if (((b & mask) == 0) && (b_prev & mask) != 0) {
				usb_keyboard_press(KEY_B, KEY_SHIFT);
				usb_keyboard_press(number_keys[i], 0);
				reset_idle = 1;
			}
			if (((d & mask) == 0) && (d_prev & mask) != 0) {
				usb_keyboard_press(KEY_D, KEY_SHIFT);
				usb_keyboard_press(number_keys[i], 0);
				reset_idle = 1;
			}
			mask = mask << 1;
		}
		// if any keypresses were detected, reset the idle counter
		if (reset_idle) {
			// variables shared with interrupt routines must be
			// accessed carefully so the interrupt routine doesn't
			// try to use the variable in the middle of our access
			cli();
			idle_count = 0;
			sei();
		}
		// now the current pins will be the previous, and
		// wait a short delay so we're not highly sensitive
		// to mechanical "bounce".
		b_prev = b;
		d_prev = d;
		_delay_ms(2);
	}
}

// This interrupt routine is run approx 61 times per second.
// A very simple inactivity timeout is implemented, where we
// will send a space character.
ISR(TIMER0_OVF_vect)
{
	idle_count++;
	if (idle_count > 61 * 8) {
		idle_count = 0;
		usb_keyboard_press(KEY_SPACE, 0);
	}
}


