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
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"
#include "gamepad.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_ON		(PORTD &= ~(1<<6))
#define LED_OFF		(PORTD |= (1<<6))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

volatile int ready = 1;

int main(void)
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

	// Configure timer 0 to generate a timer overflow interrupt every
	// 256*1024 clock cycles, or approx 61 Hz when using 16 MHz clock
	// This demonstrates how to use interrupts to implement a simple
	// inactivity timeout.
    // Timer 0 configuration (~60Hz)
	TCCR0A = 0x00;  // Normal mode
	TCCR0B = 0x05;  // Clock/1024
	TIMSK0 = (1<<TOIE0);

	while (1) {
        while (!ready);  // Block until the next cycle (~60Hz)
        cli();
        ready = 0;
        sei();

        // Read pressed buttons from gamepad interface
        gamepad_read();

	}
}

// This interrupt routine is run approx 61 times per second.
// A very simple inactivity timeout is implemented, where we
// will send a space character.
ISR(TIMER0_OVF_vect)
{
    ready = 1;
}
