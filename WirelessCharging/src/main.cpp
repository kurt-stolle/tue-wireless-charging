// Includes
#include "charger.h"
#include <stdio.h>
#include <stdlib.h>

static int compare(const void *a , const void* b){
	return ( *(double*)a - *(double*) b);
}

// Main function
int main(void) {
  // Initialize board  SystemCoreClockUpdate();
  Board_Init();

  // Print something
  DEBUGOUT("Initializing Charger");


  // Initialize the charger
  Charger *c = new Charger();

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
		  c->Delay(100);
	  }

	  // Take some averages in the duty cycle
	  /*int count = 0;
	  static const int avg = 10;
	  long double I_total = 0;
	  long double V_total = 0;

	  for (count = 0; count < avg; count++) {
		// Wait until we're not near a switch
		while (c->NearSwitch()) {}

		c->GetVI(&V, &I);

		// Add
		I_total += I;
		V_total += V;

	  }

	  // Calculate average
	  I = (double) (I_total / count);
	  V = (double) (V_total / count);*/

	  	  int count = 0;
	  	  static const int avg = 3;
	  	  double V_total[avg];

	  	  for (count = 0; count < avg; count++) {
	  		// Wait until we're not near a switch
	  		while (c->NearSwitch()) {}


	  		c->GetVI(&V, &I);

	  		V_total[count] = V;

	  	  }

	  	  qsort(V_total, avg, sizeof(double), compare);

	  	  // Compute average
	  	  V = V_total[1];
	  // Calculate power V*I
	  double power = V ;

	  // Fetch cycle from charger
	  float cycle = c->GetBoostConverterDutyCycle();
	  float cycleMax = (float) (1 - (V / 60));

	  // Decide whether to decrease, increase or keep cycle
	  if (power < prevPower || V > 60) {
		  // Decrease duty cycle when power is less than previous power and when I or V is greater than 10 and 60 resp.
		c->SetBoostConverterDutyCycle(cycle - 0.04f);
	  } else if (power > prevPower && c->GetBoostConverterDutyCycle() < cycleMax) {
		  // Increase duty cycle
		c->SetBoostConverterDutyCycle(cycle + 0.04f);
	  }

	  // Save power for previous iteration
	  prevPower = power;

	  c->Delay(600);
	} else if (c->IsCharging()) {
	  c->StopCharging(); // Stop charging if no load is found
	  c->Delay(100);
	}
  }

  // Exit without error
  return 0;
}
