/*
 *  Created on: Jan 18, 2022
 *      Author: Joe Krachey
 */

#ifndef LEDS_H__
#define LEDS_H__

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

// Pin definitions for the ECE453 Staff Dev board
#define PIN_LED_RED 		P10_3
#define PIN_LED_GREEN 	    P10_4
#define PIN_LED_YELLOW 		P10_5
#define PIN_LED_BLUE 		P10_6

/* Initializes the IO functions */
void leds_init(void);

#endif 
