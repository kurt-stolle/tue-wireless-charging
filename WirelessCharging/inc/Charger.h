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
	Charger();

	void Enable();
	void Disable();

	bool IsCharging();

	bool DetectLoad();
private:
	bool charging;

};

#endif /* CHARGER_H_ */
