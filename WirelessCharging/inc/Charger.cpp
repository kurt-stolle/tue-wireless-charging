/*
 * Charger.cpp
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */


#include <Charger.h>

// Constructor
Charger::Charger() {
  // Initialize ADC
  initADC();

  // Initialize PWM
  initPWM();

  // Power calculation pins
  Chip_IOCON_PinMux(LPC_IOCON, 0, 24, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.24 in AD0.1
  Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.25 in AD0.2

  // Load detection pin
  Chip_IOCON_PinMux(LPC_IOCON, 0, 23, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.23 in AD0.0

  // Inverter pins
  Chip_IOCON_PinMux(LPC_IOCON, 2, 1, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.1 in PWM2 mode
  Chip_IOCON_PinMux(LPC_IOCON, 2, 3, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.3 in PWM4 mode
  SetInverterDutyCycle(0.5f);

  // Boost converter pin
  Chip_IOCON_PinMux(LPC_IOCON, 2, 5, IOCON_MODE_INACT, IOCON_FUNC1);    // Select pin P2.5 in PWM6 mode

  // By default, do not charge
  StopCharging();
}

// Initialize PWM
void Charger::initPWM(){
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);

  PWM = ((pwm_t *) LPC_PWM1_BASE);                  // Cast region on memory to pwm_t pointer
  PWM->TCR |= 1 << 0;                               // Timer enable bit
  PWM->TCR |= 1 << 3;                               // PWM enable bit

  PWM->PCR = 0x0;                                  // All zeros
  PWM->PCR |= (1 << 2) | (1 << 4) | (1 << 6);      // Enable double-edged mode on PWM 2,4 and 6
  PWM->PCR |= (1 << 10) | (1 << 12) | (1 << 14);   // Enable PWM 2,4 and 6

  PWM->MR0 = PWMCycleTime;                         // PWM time to 100
  PWM->LER = PWMLatchEnable;                       // Push new MR0 value
}

// Initialize ADC
void Charger::initADC(){
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ADC);

  // Enable channels
  Chip_ADC_Init(LPC_ADC, &ADCSetup);
  Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADCBitrate);
  Chip_ADC_SetStartMode(LPC_ADC, ADC_NO_START, ADC_TRIGGERMODE_RISING); // Must be set for burst mode readings
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE); // Load detect
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE); // V
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE); // I
}

 //StopCharging starts the charger
void Charger::StopCharging() {
  // Set the charging flag
  charging = false;

  // Set the duty cycle of the PWM going to the boost converter to 0
  SetBoostConverterDutyCycle(0.0f);

  // Enable the LED indicator
  Board_LED_Set(0, false);
}

// StartCharging stops the charger
void Charger::StartCharging() {
  // Set the charging flag
  charging = true;

  // Set the duty cycle of the PWM going to the boost converter to 0.5
  SetBoostConverterDutyCycle(0.5f);
  SetInverterDutyCycle(0.5f); //initial value to start dc/ac conversion

  // Disable LED indicator
  Board_LED_Set(0, true);

  // Allow some settling time
  for(int i=0; i < CPUFrequency / 100; i++){}
}

void Charger::GetVI(double* V, double* I) {
  if (!IsCharging()) {
	*I = 0;
	*V = 0;
	return;
  }

  // Allocate variables to read into
  uint16_t temp;
  uint32_t dataCurrent = 0;
  uint32_t dataVoltage = 0;

  // Enable burst mode - start conversions
  Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);

  // Wait for A/D conversion to complete on CH2
  // One measurement takes 64 cycles
  for (uint16_t i = 0; i < powerMeasurementAverages; i++){
    // Wait for A/D conversion to complete on CH1
    while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}

    // Read the value of the ADC on CH1
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &temp);
    dataVoltage += (uint32_t) temp;

    while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_DONE_STAT) != SET) {}

    // Read the value of the ADC on CH2
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &temp);
    dataCurrent += (uint32_t) temp;
  }

  // Disable burst mode
  Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);

  // Take the average value
  dataCurrent/=powerMeasurementAverages;
  dataVoltage/=powerMeasurementAverages;


  // Parse sensor readings: 3V3 * data/2^12
  *V = (dataVoltage * 3.3) / 4096.0;
  *V *= 70.0/3.3; // Voltage is scaled down from 70V to 3V3

  *I = (dataCurrent * 3.3) / 4096.0;
  *I -= 2.5; // Offset of 2.5
  *I /= 0.04; // 40mV/A
}

// IsCharging indicates whether charging has started
bool Charger::IsCharging() {
  return charging;
}

// DetectLoad will send out a pulse and measure the response of the sensor
bool Charger::IsLoadPresent() {
  return true;
}

float Charger::GetInverterDutyCycle(){
	return dutyInverter;
}

float Charger::GetBoostConverterDutyCycle(){
	return dutyBoost;
}
// SetInverterDutyCycle sets the duty cycle of the double PWM signal that should be wired to the inverter
void Charger::SetInverterDutyCycle(float ratio) {
dutyInverter = ratio;

  PWM->MR1 = 0;                                    // PWM2 set at 0
  PWM->MR2 = (uint32_t) (PWMCycleTime * (ratio - 0.01));    // PWM2 reset
  PWM->MR3 = (uint32_t) (PWMCycleTime * (ratio));    // PWM4 set
  PWM->MR4 = (uint32_t) PWMCycleTime * (1 - 0.01);                         // PWM4 reset at T-1
  PWM->LER = PWMLatchEnable;                       // Push new MR1-4 values
}

// SetBoostConverterDutyCycle sets the duty cycle of the PWM signal that should be wired to the boost converter
void Charger::SetBoostConverterDutyCycle(float ratio) {
	dutyBoost = ratio;

  PWM->MR5 = 0;                                    // PWM6 set at 0
  PWM->MR6 = (uint32_t) (PWMCycleTime * ratio);    // PWM6 reset
  PWM->LER = PWMLatchEnable;                       // Push new MR5-6 values
}

