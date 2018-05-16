/*
 * Charger.h
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */

#include "board.h"
#include "pwm_17xx_40xx.h"

#ifndef CHARGER_H_
#define CHARGER_H_


class Charger {
public:
	// Constructor
	Charger();

	// Programming interface
	void Enable();
	void Disable();

	bool IsCharging();

	bool DetectLoad();
private:
	// State
	bool charging;

	// Analog-to-Digital Converter
	static const int ADCBitrate = 200000;
	ADC_CLOCK_SETUP_T ADCSetup;
};

#endif /* CHARGER_H_ */
