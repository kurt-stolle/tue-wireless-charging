// Includes
#include "charger.h"
#include <stdio.h>

// Main function
int main(void) {
  // REQUIRED SETUP
  SystemCoreClockUpdate();

  // Print something
  printf("Initializing Charger");


  // Initialize the charger
  Charger* c = new Charger();

  // Start an infinite loop
  // One iteration of this loop can be viewed graphically in the provided flowchart
  double prevPower = 0.0;
  double I, V;
  for (;;) {
	// Check whether a load is present
	if (c->IsLoadPresent()) {
	  // If a load is present and the device is not charging, then start charging
	  if (!c->IsCharging()) {
		c->StartCharging();
	  }

	  // Measure V and I from sensors
	  c->GetVI(&V, &I);

	  // Calculate power V*I
	  double power = V * I;

	  // Fetch cycle from charger
	  float cycle = c->GetBoostConverterDutyCycle();
	  float cycleMax = (float) (1-(V/60));
	  if (cycleMax > 0.9f) {
		cycleMax = 0.9f;
	  }

	  // Decide whether to decrease, increase or keep cycle
	  if (power < prevPower || I > 10 || V > 60) {
		// Decrease duty cycle when power is less than previous power and when I or V is greater than 10 and 60 resp.
		c->SetBoostConverterDutyCycle(cycle - 0.01f);
	  } else if (power > prevPower && c->GetBoostConverterDutyCycle() < cycleMax){
		// Increase duty cycle up to a limit of 0.9
		c->SetBoostConverterDutyCycle(cycle + 0.01f);
	  }

	  // Save power for previous iteration
	  prevPower = power;
	} else if (c->IsCharging()) {
	  c->StopCharging(); // Stop charging if no load is found
	}
  }

  // Exit without error
  return 0;
}
