/*
 * Charger.h
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */

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
	static int ADCBitrate = 200000;
	ADC_CLOCK_SETUP_T ADCSetup;
};

#endif /* CHARGER_H_ */
