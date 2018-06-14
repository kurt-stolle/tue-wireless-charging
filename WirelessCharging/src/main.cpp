// Includes
#include "charger.h"
#include <stdio.h>

// Charger class
static Charger* c;

// Main function
int main(void) {
  // REQUIRED SETUP
  SystemCoreClockUpdate();
  Board_Init();


  // Print something
  printf("Initializing Charger");


  // Initialize the charger
  c = new Charger();

  // Start an infinite loop
  // One iteration of this loop can be viewed graphically in the provided flowchart
  double power = 0.0;
  for (;;) {
	// Check whether a load is present
	if (c->IsLoadPresent()) {
	  // If a load is present and the device is not charging, then start charging
	  if (!c->IsCharging()) {
		c->StartCharging();
	  }

	  // Debug print
	  printf("Load is present, resuming MPPT");

	  // Perform some iterations the MPPT algorithm
	  for (int a = 0; a < 10; a++) {
		double I,V;
		c->GetVI(&V,&I);

		double measure = V*I;
		float cycle = c->GetBoostConverterDutyCycle();
		if (measure < power) {
		  c->SetBoostConverterDutyCycle(cycle - 0.01f);
		} else if (measure > power && V < 60 && c->GetBoostConverterDutyCycle() < 0.9f && I < 10) {
			c->SetBoostConverterDutyCycle(cycle + 0.01f);
		}
		power = measure; // Save this for next iteration
	  }

	} else if (c->IsCharging()) {
	  c->StopCharging(); // Stop charging if no load is found
	}
  }

  // Exit without error
  return 0;
}
