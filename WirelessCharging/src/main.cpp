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
    	for(int a = 0; a < 10; a++){
    	    	//send pulse
    	    	//read sensor
    	    	//store sensor value
    	    	c->SendPulse();
    	    	c->ReadSensor();
    	    }
    	    //calculate average sensor value
    	    //calculate load
    	    c->LoadCalculation();

    	// Measure the
    	// Detect a load
    	if (c->DetectLoad()){
    		c->Enable(); // Start charging if a load is found
    		c->ReadSensor();//read sensor
    		c->MPP();//maximum power point
    		c->Enable();//adjust Inverter and boost converter


    	} else if (c->IsCharging()) {
    		c->Disable(); // Stop charging if no load is found
    	}

    	// Increment counter (accesses memory)
        i++ ;
    }

    // Exit without error
    return 0 ;
}
