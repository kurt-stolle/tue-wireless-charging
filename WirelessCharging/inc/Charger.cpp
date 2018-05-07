/*
 * Charger.cpp
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */

#include "board.h"
#include <Charger.h>

#include <cr_section_macros.h>

// Charger is a class that performs hardware operations on the microcontroller
Charger::Charger() {
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
	Board_Init();

	// Indicate that by default charging is disabled
	Board_LED_Set(0, false);
	charging = false;

	// Select pin P2.0 in PWM1.1 mode
	Chip_IOCON_PinMux(LPC_IOCON, 4, 1, IOCON_MODE_INACT, IOCON_FUNC1);

}

// Enable starts the charger
void Charger::Enable() {
	// Set the charging flag
	charging = true;

	// Enable the LED indicator
	Board_LED_Set(0, true);
}

// Disable stops the charger
void Charger::Disable() {
	// Disable charging flag
	charging = false;

	// Disable LED indicator
	Board_LED_Set(0, false);
}

// IsCharging indicates whether charging has started
bool Charger::IsCharging() {
	return charging;
}

// DetectLoad will send out a pulse and measure the response of the sensor
bool Charger::DetectLoad() {
	return true;
}
