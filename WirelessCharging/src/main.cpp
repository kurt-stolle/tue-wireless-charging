/*
===============================================================================
 Name        : main.c
 Author      : K.H.W. Stolle
 Version     : 0.1
 Copyright   : 2018
 Description : main definition
===============================================================================
*/

#include "charger.h"

int main(void) {
    // Initialize the charger
    auto c = new Charger();

    // Start an infinite loop
    // One iteration of this loop can be viewed graphically in the provided flowchart
    volatile static int i = 0 ;
    while(1) {
    	// Measure the
    	// Detect a load
    	if (c->DetectLoad()){
    		c->Enable(); // Start charging if a load is found




    	} else if (c->IsCharging()) {
    		c->Disable(); // Stop charging is no load is found
    	}

    	// Increment counter (accesses memory)
        i++ ;
    }

    // Exit without error
    return 0 ;
}
