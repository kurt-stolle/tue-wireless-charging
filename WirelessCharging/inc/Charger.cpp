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

  Chip_IOCON_PinMux(LPC_IOCON, 0, 2, IOCON_MODE_INACT, IOCON_FUNC2); // Select pins
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH6, ENABLE);

  // LOAD DETECTION
  Chip_IOCON_PinMux(LPC_IOCON, 1, 10, IOCON_MODE_INACT, IOCON_FUNC2);
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);

  // INVERTER PWM SIGNALS
  Chip_IOCON_PinMux(LPC_IOCON, 2, 0, IOCON_MODE_INACT, IOCON_FUNC1); // Select pin P2.0 in PWM mode
  Chip_IOCON_PinMux(LPC_IOCON, 2, 0, IOCON_MODE_INACT, IOCON_FUNC1); // Select pin P2.2 in PWM mode
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1); // Enable clock
  LPC_PWM1->TCR |= PWM_TIM_ENABLE; // Enable PWM
  LPC_PWM1->TCR |= PWM_ENABLE;
  
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
