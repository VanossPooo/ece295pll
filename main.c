/*
 * Si5351_SkeletonCode.c
 * This project contains the skeleton code to get started with the 
 * Si5351 PLL chip
 *
 * Created: 2023-02-28 4:39:21 PM
 * Author : StewartPearson
 */ 

#define F_CPU 1000000

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "./Si5351.h"
#include "./twi.h"


int main(void)
{
	bool enabled = true;
	
	
	// Initialize the I2C Bus
	twi_init();
	
	//TODO: Output a 10 MHz frequency using the Si5351.c commands. Also, you must complete the write function in Si5351.c.
    
	while(1)
	{
	}
}

