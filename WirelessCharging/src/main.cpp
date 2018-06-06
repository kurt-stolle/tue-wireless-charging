// Includes
#include "charger.h"

// Definitions
#define LOAD_DETECTION_ITERATIONS 4

// Main function
int main(void) {
	// REQUIRED SETUP
	SystemCoreClockUpdate();
	Board_Init();

	// Initialize the charger
	auto c = new Charger();

	// Start an infinite loop
	// One iteration of this loop can be viewed graphically in the provided flowchart
	volatile static int i = 0;
	while (1) {
		// Measure the
		// Detect a load
		if (c->IsLoadPresent()) {
			c->StartCharging(); // Start charging if a load is found

			while(1){
			double p = c->GetPower();

			if (){//to do:add condition for a lower duty cycle
				//to do: decrease duty cycle
			}
			else if(){//to do:add condition for a higher duty cycle
				//To do: increase duty cycle
				}
			//To do: check whether load is still there
			}
		} else if (c->IsCharging()) {
			c->StopCharging(); // Stop charging if no load is found
		}

		// Increment counter (accesses memory)
		i++;
	}

	// Exit without error
	return 0;
}
