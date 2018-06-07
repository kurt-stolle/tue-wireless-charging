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
	double p_old;

  	volatile static int i;
	while (1) {
		// Measure the
		// Detect a load
		if (c->IsLoadPresent()) {
		  	if (!c->IsCharging()){
			  c->StartCharging(); // Start charging if a load is found and not already charging
			  for(; i < 200000; i++){ //wait one second

			  }
			  p_old = c->GetPower();
			}
		  	for (int a=0; a<10; a++){
		  		double p = c->GetPower();
		  		if(p != p_old){//condition for a lower duty cycle
		  			if (p_old > p){
		  				c->SetBoostConverterDutyCycle(c->GetBoostConverterDutyCycle() - 0.05f);
		  			}
		  			else if(p > p_old){
		  				if(c->GetBoostConverterDutyCycle() < 0.9f){
		  				c->SetBoostConverterDutyCycle(c->GetBoostConverterDutyCycle() + 0.05f);
		  				}
		  			}
		  		}
		  		p_old = p;
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
