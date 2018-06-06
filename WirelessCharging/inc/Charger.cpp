/*
 * Charger.cpp
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */


#include <Charger.h>


// Constructor
Charger::Charger() {
  // A/D CONVERTERS SETUP
  Chip_ADC_Init(LPC_ADC, &ADCSetup);
  Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADCBitrate);
  Chip_ADC_SetStartMode(LPC_ADC, ADC_NO_START, ADC_TRIGGERMODE_RISING); // Must be set for burst mode readings

  // PWM SETUP
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);            // Enable clock
  PWM = ((pwm_t *) LPC_PWM1_BASE);                            // Cast region on memory to pwm_t pointer
  PWM->TCR |= 1 << 0;                                         // Timer enable bit
  PWM->TCR |= 1 << 3;                                         // PWM enable bit
  PWM->MR0 = PWMCycleTime;                                    // PWM time to 100
  PWM->LER = (1 << 0);                                        // Push new MR0 value

  PWM->PCR = 0x0;                                             // All zeros
  PWM->PCR |= (1 << 2) | (1 << 4) | (1 << 6);                 // Enable double-edged mode on PWM 2,4 and 6
  PWM->PCR |= (1 << 10) | (1 << 12) | (1 << 14);              // Enable PWM 2,4 and 6

  // CHARGING INDICATOR SETUP
  Board_LED_Set(0, false);
  charging = false;

  // POWER CALCULATION PIN INITIALIZATION
  Chip_IOCON_PinMux(LPC_IOCON, 0, 24, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.24 in AD0.1
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);                    // Enable ADC channel 1

  Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.25 in AD0.2
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);                    // Enable ADC channel 2

  // LOAD DETECTION PIN INITIALIZATION
  Chip_IOCON_PinMux(LPC_IOCON, 0, 23, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.23 in AD0.0
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);                    // Enable ADC channel 0

  // INVERTER PWM SIGNALS SETUP AND PIN INITIALIZATION
  Chip_IOCON_PinMux(LPC_IOCON, 2, 1, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.1 in PWM2 mode
  Chip_IOCON_PinMux(LPC_IOCON, 2, 3, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.3 in PWM4 mode
  SetInverterDutyCycle(0.4f);

  // BOOST CONVERTER PWM CONTROL
  Chip_IOCON_PinMux(LPC_IOCON, 2, 5, IOCON_MODE_INACT, IOCON_FUNC1);    // Select pin P2.5 in PWM6 mode
}

 //StopCharging starts the charger
void Charger::StopCharging() {
  // Set the charging flag
  charging = false;

  // Set the duty cycle of the PWM going to the boost converter to 0
  SetBoostConverterDutyCycle(0.0f);

  // Enable the LED indicator
  Board_LED_Set(0, true);
}

// StartCharging stops the charger
void Charger::StartCharging() {
  // Set the charging flag
  charging = true;

  // Set the duty cycle of the PWM going to the boost converter to 0.5
  SetBoostConverterDutyCycle(0.5f);

  // Disable LED indicator
  Board_LED_Set(0, false);
}

double Charger::GetPower() {
  if (!IsCharging()) {
	return 0;
  }

  // Allocate variables to read into
  uint16_t temp;
  uint16_t dataCurrent = 0;
  uint16_t dataVoltage = 0;

  // Enable burst mode - start conversions
  Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);

  // Wait for A/D conversion to complete on CH2
  for (uint16_t i = 0; i < powerMeasurementAverages; i++){
    while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_DONE_STAT) != SET) {}


    // Read the value of the ADC on CH2
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &temp);
    dataCurrent += temp;



    // Wait for A/D conversion to complete on CH1
    while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}

    // Read the value of the ADC on CH1
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &temp);
    dataVoltage += temp;
  }

  // Disable burst mode - stop reading
  Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);

  // Take the average value
  dataCurrent/=powerMeasurementAverages;
  dataVoltage/=powerMeasurementAverages;


  // Parse sensor readings: 3V3 * data/2^12
  double V = (dataVoltage * 3.3) / 4096.0;
  V *= 2; // Scale of actual voltage level

  double I = (dataCurrent * 3.3) / 4096.0;
  I -= 2.5; // Offset of 2.5
  I /= 0.04; // 40mV/A

  // Return power
  return V * I;
}

// IsCharging indicates whether charging has started
bool Charger::IsCharging() {
  return charging;
}

// DetectLoad will send out a pulse and measure the response of the sensor
bool Charger::IsLoadPresent() {

  return true;
}

// SetInverterDutyCycle sets the duty cycle of the double PWM signal that should be wired to the inverter
void Charger::SetInverterDutyCycle(float ratio) {
  PWM->MR1 = 0;                                    // PWM2 set at 0
  PWM->MR2 = (uint32_t) (PWMCycleTime * ratio);    // PWM2 reset
  PWM->MR3 = (uint32_t) (PWMCycleTime * ratio);    // PWM4 set
  PWM->MR4 = PWMCycleTime;                        // PWM4 reset at T-1
  PWM->LER = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);    // Push new MR1-4 values
}

// SetBoostConverterDutyCycle sets the duty cycle of the PWM signal that should be wired to the boost converter
void Charger::SetBoostConverterDutyCycle(float ratio) {
  PWM->MR5 = 0;                                    // PWM6 set at 0
  PWM->MR6 = (uint32_t) (PWMCycleTime * ratio);    // PWM6 reset
  PWM->LER = (1 << 0) | (1 << 5) | (1 << 6);            // Push new MR5-6 values
}

