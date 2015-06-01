/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2014  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

#include "ee.h"
#include "Arduino.h"
#include "can.h"
#include "mcp_can.h"

MCP_CAN CAN(9);


void setup_every250uS()
{
// set prescalar to "/32" = 011
TCCR2B = _BV(CS21) | _BV(CS20);

// set WGM to CTC mode (010)
// In this mode Timer2 counts up until it matches OCR2A
// we need to use OCR2A instead of the overflow so we can interrupt
// more often
TCCR2A = _BV(WGM21);

// These are actual measurements from oscope:
//   0 : 40409.6hz
//  10 : 22730.3
// 100 :  2475.6
// 124 :  2000.3
// 125 :  1984.4
OCR2A = 124;

// When the OCR2A register matches the Timer2 count, cause an interrupt
TIMSK2 = _BV(OCIE2A);
}

void setup(void)
{
	// initialize the digital pin as an output.
	pinMode(13, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(11, OUTPUT);


	CAN_initialize(CAN_500KBPS);

	Serial.begin(9600);
	Serial.println("Serial Communication Established");

	setup_every250uS();   // Used for ISR.
}

int main(void)
{
	EE_mcu_init();

	init();

#if defined(USBCON)
	USBDevice.attach();
#endif
	
	setup();

	StartOS(appmode1);

	for (;;) {

		if (serialEventRun) serialEventRun();
	}

	return 0;

}
