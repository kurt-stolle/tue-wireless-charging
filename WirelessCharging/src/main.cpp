/*
===============================================================================
 Name        : main.c
 Author      : K.H.W. Stolle
 Version     : 0.1
 Copyright   : 2018
 Description : main definition
===============================================================================
*/

#include "board.h"

#include <cr_section_macros.h>

int main(void) {
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
    Board_Init();

    // Set the LED to the state of "On"
    Board_LED_Set(0, true);

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }

    // Exit without error
    return 0 ;
}
