/*
 * Charger.cpp
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */

#include <Charger.h>

Charger::Charger() {
	// A/D CONVERTERS SETUP
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADC_BITRATE);

	// PWM SETUP
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);                    // Enable clock
	LPC_PWM1->TCR |= 1 << 0;                                            // Timer enable bit
	LPC_PWM1->TCR |= 1 << 3;                                            // PWM enable bit
	LPC_PWM1->MR0 = PWM_CYCLE_TIME;										// PWM time to 100

	LPC_PWM1->PCR = 0x0;												// All zeros
	LPC_PWM1->PCR |= (1 << 2) | (1 << 4) | (1 << 6);					// Enable double-edged mode on PWM 2,4 and 6
	LPC_PWM1->PCR |= (1 << 10)| (1 << 12)| (1 << 14);					// Enable PWM 2,4 and 6

	// CHARGING INDICATOR SETUP
	Board_LED_Set(0, false);
	charging = false;

	// POWER CALCULATION PIN INITIALIZATION
	Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_INACT, IOCON_FUNC1);	// Select pin P0.25 in AD0.2
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);					// Enable ADC channel 2

	Chip_IOCON_PinMux(LPC_IOCON, 0, 24, IOCON_MODE_INACT, IOCON_FUNC1);	// Select pin P0.24 in AD0.1
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);					// Enable ADC channel 1

	// LOAD DETECTION PIN INITIALIZATION
	Chip_IOCON_PinMux(LPC_IOCON, 0, 23, IOCON_MODE_INACT, IOCON_FUNC1);	// Select pin P0.23 in AD0.0
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);					// Enable ADC channel 0

	// INVERTER PWM SIGNALS SETUP AND PIN INITIALIZATION
	Chip_IOCON_PinMux(LPC_IOCON, 2, 1, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.1 in PWM2 mode
	Chip_IOCON_PinMux(LPC_IOCON, 2, 3, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.3 in PWM4 mode

	LPC_PWM1->MR1 = 0; 												// PWM2 set at 0
	LPC_PWM1->MR2 = PWM_CYCLE_TIME/2; 								// PWM2 reset at T/2
	LPC_PWM1->MR3 = PWM_CYCLE_TIME/2;								// PWM4 set at T/2
	LPC_PWM1->MR4 = PWM_CYCLE_TIME-1;								// PWM4 reset at T-1
	LPC_PWM1->LER = (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4); 	// Push new MR0-4 values

	// BOOST CONVERTER PWM CONTROL
	Chip_IOCON_PinMux(LPC_IOCON, 2, 5, IOCON_MODE_INACT, IOCON_FUNC1); 	// Select pin P2.5 in PWM6 mode
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

double Charger::GetPower() {
	uint16_t dataVoltage;
	uint16_t dataCurrent;

	 // Wait for A/D conversion to complete
	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_DONE_STAT) != SET) {}

	// Read the value of the ADC on CH2 into dataCurrent
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &dataCurrent);

    // Wait for A/D conversion to complete
	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}

	// Read the value of the ADC on CH1 into dataVoltage
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataVoltage);

    // Parse sensor readings
	double V = (dataVoltage*3.3)/(2^12);
	double I = (dataCurrent*3.3)/(2^12);

    // Return power
	return V*I;
}

// IsCharging indicates whether charging has started
bool Charger::IsCharging() {
	return charging;
}

// DetectLoad will send out a pulse and measure the response of the sensor
bool Charger::DetectLoad() {
	return true;
}
