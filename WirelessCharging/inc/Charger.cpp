/*
 * Charger.cpp
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */

#include <Charger.h>

// Charger is a class that performs hardware operations on the microcontroller
Charger::Charger() {
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
	Board_Init();

	// Perform ADC setup
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADCBitrate);

	// Indicate that by default charging is disabled
	Board_LED_Set(0, false);
	charging = false;

	// Select pin P2.0 in PWM1.1 mode
	Chip_IOCON_PinMux(LPC_IOCON, 4, 1, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_PWM_Init(LPC_PWM1);
	Chip_PWM_Enable(LPC_PWM1);

	// Select pin P0.2 in AD0.7 mode for power measurement
	Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH7, ENABLE);

	// Select pin P0.3 in AD0.6 mode for power measurement
	Chip_IOCON_PinMux(LPC_IOCON, 0, 2, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH6, ENABLE);

	// Select pin P0.25 in AD0.2 mode for load detection
	Chip_IOCON_PinMux(LPC_IOCON, 1, 10, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
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
