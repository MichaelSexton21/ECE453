/*
 * ece453.c
 *
 *  Created on: Jan 18, 2022
 *      Author: Joe Krachey
 */

#include "sw2.h"

volatile bool ALERT_SW2 = false;

/* SW2 Handler Struct */
cyhal_gpio_callback_data_t sw2_cb_data =
{
    .callback     = sw2_handler,
    .callback_arg = (void*)NULL
};

/***********************************************************
* Handler for SW2
************************************************************/
void sw2_handler(void* handler_arg, cyhal_gpio_event_t event)
{
	ALERT_SW2= true;

    /*ADD CODE to toggle the LEDs ON/Off) */

	cyhal_gpio_toggle(PIN_LED_RED);
	cyhal_gpio_toggle(PIN_LED_GREEN);
	cyhal_gpio_toggle(PIN_LED_YELLOW);
	cyhal_gpio_toggle(PIN_LED_BLUE);
}

/***********************************************************
* Initialize SW2 Interrupts
************************************************************/
static void sw2_irq_init(void)
{
	// Enable the interrupt for the CapSense Change IRQ
    cyhal_gpio_register_callback(
			PIN_SW2, 		        	// Pin
			&sw2_cb_data);			    // Handler Callback Info

    cyhal_gpio_enable_event(
			PIN_SW2, 		        	// Pin
			CYHAL_GPIO_IRQ_FALL, 	    // Event
			INT_PRIORITY_SW2, 	        // Priority
			true);			            // Enable Event
}

/*****************************************************
* Function Name: io_sw2_init
******************************************************
* Summary:
* Initializes the IO pin used to control SW2 on the 
* CYBLE-PROTO-063
*
* Parameters:
*  void
*
* Return:
*
*
*****************************************************/
static void sw2_io_init(void)
{
	cyhal_gpio_init(
			PIN_SW2,                // Pin
				CYHAL_GPIO_DIR_INPUT,      // Direction
				CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
				true);				        // InitialValue
}


/*****************************************************
* Function Name: sw2_init
******************************************************
* Summary:
* Initializes the IO pin used to control SW2 on the
* CYBLE-PROTO-063
*
* Parameters:
*  void
*
* Return:
*
*
*****************************************************/
void sw2_init(void)
{
	sw2_io_init();
	sw2_irq_init();
}
