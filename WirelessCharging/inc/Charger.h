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

// Define region in memory of PWM driver
#define PWM ((pwm_t *) LPC_PWM1_BASE)

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

  float GetBoostConverterDutyCycle();

  void SetBoostConverterDutyCycle(float ratio);

  void GetVI(double*,double*);

  void Delay(unsigned int ms);
 private:
  // Initialization
  void initPWM();
  void initADC();
  void initGPIO();

  // Constants
  const double PWMCycleTime = 400 * 60.0/45.3; // Cycle time of the PWM, 400 = 60kHz.
  const uint16_t PWMLatchEnable = (uint16_t) (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6); // MR0-6
  const uint16_t ADCBitrate = (uint16_t) 180000; // Bitrate of the ADC in Hz
  const uint16_t powerMeasurementAverages = 200;	 // How many ADC measurements do we take before we 'trust' the result?

  // Save duty cycles
  float dutyBoost = 0.4;

  // State
  bool charging;

  // Analog-to-Digital Converter setup
  ADC_CLOCK_SETUP_T ADCSetup;

};

#endif /* CHARGER_H_ */
