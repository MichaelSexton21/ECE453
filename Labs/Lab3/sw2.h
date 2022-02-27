/*
 * ece453.h
 *
 *  Created on: Jan 18, 2022
 *      Author: Joe Krachey
 */

#ifndef SW2_H_
#define SW2_H_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#include "leds.h"

// Macros
#define PIN_SW2				P0_4
#define INT_PRIORITY_SW2	3

// Exported Global Variables
extern volatile bool ALERT_SW2;

/* Public Function API */
void sw2_handler(void* handler_arg, cyhal_gpio_event_t event);
void sw2_init(void);


#endif
