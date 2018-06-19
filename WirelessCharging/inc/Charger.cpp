/*
 * Charger.cpp
 *
 *  Created on: 7 May 2018
 *      Author: kurts
 */


#include <Charger.h>

// Constructor
Charger::Charger() {
  // Load detection pin
  Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC0);
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 0); // Set P0.0 as input

  // Power calculation pins
  Chip_IOCON_PinMux(LPC_IOCON, 0, 24, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.24 in AD0.1
  Chip_IOCON_PinMux(LPC_IOCON, 0, 25, IOCON_MODE_PULLUP, IOCON_FUNC1);  // Select pin P0.25 in AD0.2

  // Inverter pins
  Chip_IOCON_PinMux(LPC_IOCON, 2, 1, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.1 in PWM2 mode
  Chip_IOCON_PinMux(LPC_IOCON, 2, 3, IOCON_MODE_INACT, IOCON_FUNC1);  // Select pin P2.3 in PWM4 mode

  // Boost converter pin
  Chip_IOCON_PinMux(LPC_IOCON, 2, 5, IOCON_MODE_INACT, IOCON_FUNC1);    // Select pin P2.5 in PWM6 mode

  // Initialize PWM
  initPWM();

  // Initialize ADC
  initADC();
}

// Initialize PWM
void Charger::initPWM() {
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);

  PWM->TCR = (1 << 0) | (1 << 3);                               // Timer enable bit and PWM enable bit

  // No prescalar
  PWM->PR = 0x0;

  // Reset when TC matches MR0
  PWM->MCR = (1 << 1);

  // Set match registers
  PWM->MR0 = (uint16_t) PWMCycleTime;                         // PWM time to 100

  // Set match registers for inverter
  PWM->MR1 = 0;                                    // PWM2 set at 0
  PWM->MR2 = (uint32_t) (PWMCycleTime * (0.475));    // PWM2 reset with deadtime added
  PWM->MR3 = (uint32_t) (PWMCycleTime * (0.5));    // PWM4 set
  PWM->MR4 = (uint32_t) (PWMCycleTime * (0.975));

  // Best-guess vales for MPPT
  PWM->MR5 = 0;                                    // PWM6 set at 0
  PWM->MR6 = (uint32_t) (PWMCycleTime * dutyBoost);    	// PWM6 reset

  // Trigger the latch enable, set MR values
  PWM->LER = PWMLatchEnable;                       // Push new MR0 value

  // Enable PWM 2,4 and 6 and Enable double-edged mode on PWM 2,4 and 6
  PWM->PCR = (1 << 2) | (1 << 4) | (1 << 6) | (1 << 10) | (1 << 12) | (1 << 14);

  // Set TC to 0
  PWM->TC = 0x0;

  // Small delay to propagate changes
  Delay(10);
}

// Initialize ADC
void Charger::initADC(){
  //Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ADC);

  // Enable channels
  Chip_ADC_Init(LPC_ADC, &ADCSetup);
  Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADCBitrate);
  Chip_ADC_SetStartMode(LPC_ADC, ADC_NO_START, ADC_TRIGGERMODE_RISING); // Must be set for burst mode readings
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE); // V
  Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE); // I
  Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);
}

 //StopCharging starts the charger
void Charger::StopCharging() {
  // Set the charging flag
  charging = false;

  // Set the duty cycle of the PWM going to the boost converter to 0
   PWM->MR5 = 0;                // PWM6 set at 0
   PWM->MR6 = 0;			    // PWM6 reset
   PWM->LER = PWMLatchEnable;   // Push new MR5-6 values

   Delay(10);

  // Enable the LED indicator
  Board_LED_Set(0, false);
}

// StartCharging stops the charger
void Charger::StartCharging() {
  // Set the charging flag
  charging = true;

  // Re-set the duty cycle to the expected value
  SetBoostConverterDutyCycle(dutyBoost);

  // Disable LED indicator
  Board_LED_Set(0, true);
}

void Charger::GetVI(double* V, double* I) {
  if (!IsCharging()) {
	*I = 0;
	*V = 0;
	return;
  }

  // Allocate variables to read into
  //uint16_t temp;
  uint16_t dataCurrent = 0;
  uint16_t dataVoltage = 0;

  // Read voltage and current data
  while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_DONE_STAT) != SET) {}
  Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &dataVoltage);

  while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}
  Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataCurrent);

	/*
  // Wait for A/D conversion to complete on CH2
  // One measurement takes 64 cycles
  for (uint16_t i = 0; i < powerMeasurementAverages; i++){
    // Wait for A/D conversion to complete on CH1
    while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH2, ADC_DR_DONE_STAT) != SET) {}

    // Read the value of the ADC on CH1
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &temp);
    dataVoltage += (uint32_t) temp;

    while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}

    // Read the value of the ADC on CH2
    Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &temp);
    dataCurrent += (uint32_t) temp;
  }

  // Take the average value
  dataCurrent/=powerMeasurementAverages;
  dataVoltage/=powerMeasurementAverages;
  */
  // Parse sensor readings: 3V3 * data/2^12
  *V = (dataVoltage * 3.3) / 4096.0;
  *V *= 70.0/3.3; // Voltage is scaled down from 70V to 3V3

  *I = (dataCurrent * 3.3) / 4096.0;
  *I = (*I - 2.5) / 0.04; // Offset of 2.5V and 40mV/A
}

// IsCharging indicates whether charging has started
bool Charger::IsCharging() {
  return charging;
}

// DetectLoad will send out a pulse and measure the response of the sensor
bool Charger::IsLoadPresent() {
	bool loadPresent = Chip_GPIO_ReadPortBit(LPC_GPIO, 0, 0);

  return loadPresent;
}

float Charger::GetBoostConverterDutyCycle(){
	return dutyBoost;
}

// SetBoostConverterDutyCycle sets the duty cycle of the PWM signal that should be wired to the boost converter
void Charger::SetBoostConverterDutyCycle(float ratio) {
	dutyBoost = ratio;

  PWM->MR5 = 0;                                    // PWM6 set at 0
  PWM->MR6 = (uint32_t) (PWMCycleTime * ratio);    // PWM6 reset
  PWM->LER = PWMLatchEnable;                       // Push new MR5-6 values

  Delay(10);
}

// Delay will delay computation for a while
void Charger::Delay(unsigned int ms){
  unsigned int i,j;
  for(i=0;i<ms;i++)
	for(j=0;j<50000;j++);
}

// Wait until we're around the middle of the duty cycle
bool Charger::NearSwitch() {
  uint32_t center = PWM->MR6 / 2;
  uint32_t bounds = PWMCycleTime / 12;
  uint32_t timer = PWM->TC;

  return (timer > bounds && timer < center-bounds) || (timer > center+bounds && timer < PWMCycleTime-bounds);
}
