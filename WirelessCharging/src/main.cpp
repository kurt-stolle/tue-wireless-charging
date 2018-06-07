// Includes
#include "charger.h"

// Main function
int main(void) {
	// REQUIRED SETUP
	SystemCoreClockUpdate();
	Board_Init();

	// Initialize the charger
	volatile static auto c = new Charger();

	// Start an infinite loop
	// One iteration of this loop can be viewed graphically in the provided flowchart
  	volatile static int i;
	while (1) {
		// Measure the
		// Detect a load
		if (c->IsLoadPresent()) {
		  	if (!c->IsCharging()){
			  c->StartCharging(); // Start charging if a load is found and not already charging

			}

		  	for (int a=0; a<10; a++){
			  double p = c->GetPower();

			  if (false){//to do:add condition for a lower duty cycle
				//to do: decrease duty cycle
				c->SetInverterDutyCycle(0.4f);
			  }
			  else if(false){//to do:add condition for a higher duty cycle
				//To do: increase duty cycle
				c->SetInverterDutyCycle(0.4f);
			  }
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
