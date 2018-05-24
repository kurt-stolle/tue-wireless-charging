/*
 * Charger.h
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */

#include "board.h"

#ifndef CHARGER_H_
#define CHARGER_H_

// Define the memory layout of the PWM
// Nice tutorial for PWM setup: https://exploreembedded.com/wiki/LPC1768:_PWM
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
} LPC_PWM_T;

// Cast the region of memory to the LPC_PWM_% type
#define LPC_PWM1  ((LPC_PWM_T*) LPC_PWM1_BASE)
#define ADC_BITRATE 2000000
#define PWM_CYCLE_TIME 100

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
  ADC_CLOCK_SETUP_T ADCSetup;

};

#endif /* CHARGER_H_ */
