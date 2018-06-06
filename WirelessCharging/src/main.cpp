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
		if (c->DetectLoad()) {
			c->Enable(); // Start charging if a load is found

			double p = c->GetPower();


		} else if (c->IsCharging()) {
			c->Disable(); // Stop charging if no load is found
		}

		// Increment counter (accesses memory)
		i++;
	}

	// Exit without error
	return 0;
}
