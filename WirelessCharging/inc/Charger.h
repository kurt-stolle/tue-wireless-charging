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

#define LPC_PWM1  ((LPC_PWM_T*) LPC_PWM1_BASE)

typedef struct {                                        /*!< TIMERn Structure       */
  __IO uint32_t IR;                               /*!< Interrupt Register. The IR can be written to clear interrupts. The IR can be read to identify which of eight possible interrupt sources are pending. */
  __IO uint32_t TCR;                              /*!< Timer Control Register. The TCR is used to control the Timer Counter functions. The Timer Counter can be disabled or reset through the TCR. */
  __IO uint32_t TC;                               /*!< Timer Counter. The 32 bit TC is incremented every PR+1 cycles of PCLK. The TC is controlled through the TCR. */
  __IO uint32_t PR;                               /*!< Prescale Register. The Prescale Counter (below) is equal to this value, the next clock increments the TC and clears the PC. */
  __IO uint32_t PC;                               /*!< Prescale Counter. The 32 bit PC is a counter which is incremented to the value stored in PR. When the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable and controllable through the bus interface. */
  __IO uint32_t MCR;                              /*!< Match Control Register. The MCR is used to control if an interrupt is generated and if the TC is reset when a Match occurs. */
  __IO uint32_t MR0;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
  __IO uint32_t MR1;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
  __IO uint32_t MR2;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
  __IO uint32_t MR3;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
  __IO uint32_t CCR;                              /*!< Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place. */
  __IO uint32_t CR[2];                    /*!< Capture Register. CR is loaded with the value of TC when there is an event on the CAPn.0 input. */
  __IO uint32_t RESERVED[3];
  __IO uint32_t MR4;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
  __IO uint32_t MR5;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
  __IO uint32_t MR6;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
  __IO uint32_t PCR;
  __IO uint32_t LER;
  __IO uint32_t CTCR;
} LPC_PWM_T;

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
