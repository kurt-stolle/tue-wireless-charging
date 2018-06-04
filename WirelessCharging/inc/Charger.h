/*
 * Charger.h
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */

#include "board.h"

#ifndef CHARGER_H_
#define CHARGER_H_

// Define the memory layout of the PWM region
typedef struct {
  __IO uint32_t IR;
  __IO uint32_t TCR;
  __IO uint32_t TC;
  __IO uint32_t PR;
  __IO uint32_t PC;
  __IO uint32_t MCR;
  __IO uint32_t MR0;
  __IO uint32_t MR1;
  __IO uint32_t MR2;
  __IO uint32_t MR3;
  __IO uint32_t CCR;
  __IO uint32_t CR[2];
  __IO uint32_t RESERVED[3];
  __IO uint32_t MR4;
  __IO uint32_t MR5;
  __IO uint32_t MR6;
  __IO uint32_t PCR;
  __IO uint32_t LER;
} pwm_t;

// Charger is a class that performs hardware operations on the MC
class Charger {
 public:
  // Constructor
  Charger();

  // Programming interface
  void StartCharging();
  void StopCharging();


  bool IsCharging();
  bool IsLoadPresent();

  void SetInverterDutyCycle(float ratio);
  void SetBoostConverterDutyCycle(float ratio);

  double GetPower();
 private:
  // Constants
  const uint16_t PWMCycleTime = (uint16_t) 400;
  const uint16_t ADCBitrate = (uint16_t) 120000;
  const uint16_t powerMeasurementAverages = 40;

  // PWM driver
  volatile pwm_t* PWM;

  // State
  bool charging;

  // Analog-to-Digital Converter setup
  ADC_CLOCK_SETUP_T ADCSetup;

};

#endif /* CHARGER_H_ */
