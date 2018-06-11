// Includes
#include "charger.h"

// Charger class
static Charger* c;

// Main function
int main(void) {
  // REQUIRED SETUP
  SystemCoreClockUpdate();
  Board_Init();

  // Initialize the charger
  c = new Charger();

  // Start an infinite loop
  // One iteration of this loop can be viewed graphically in the provided flowchart
  double power = 0.0;
  volatile static int i;
  for (;;) {
	// Check whether a load is present
	if (c->IsLoadPresent()) {
	  // If a load is present and the device is not charging, then start charging
	  if (!c->IsCharging()) {
		c->StartCharging();
	  }

	  // Perform some iterations the MPPT algorithm
	  for (int a = 0; a < 10; a++) {
		double I,V;
		c->GetVI(&I,&V);

		double measure = V*I;
		if (measure < power || V > 60) {
		  c->SetBoostConverterDutyCycle(c->GetBoostConverterDutyCycle() - 0.05f);
		} else if (measure > power) {
		  if (c->GetBoostConverterDutyCycle() < 0.9f) {
			c->SetBoostConverterDutyCycle(c->GetBoostConverterDutyCycle() + 0.05f);
		  }
		}
		power = measure; // Save this for next iteration
	  }

	} else if (c->IsCharging()) {
	  c->StopCharging(); // Stop charging if no load is found
	}

	// Prevent optimization
	i++;
  }

  // Exit without error
  return 0;
}
