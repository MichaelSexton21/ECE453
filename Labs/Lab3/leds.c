/*
 * ece453.c
 *
 *  Created on: Jan 18, 2022
 *      Author: Joe Krachey
 */

#include <leds.h>

/*****************************************************
* Function Name: leds_init
******************************************************
* Summary:
* Initializes the IO pins used to control the LEDs
* on the ECE453 Dev board
*
* Parameters:
*  void
*
* Return:
*
*
*****************************************************/
void leds_init(void)
{
	/* Initialize RED LED*/
	cyhal_gpio_init(
			PIN_LED_RED,                // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			true);				        // InitialValue

    /* ADD CODE to configure the GREEN LED as an output */
	cyhal_gpio_init(
				PIN_LED_GREEN,                // Pin
				CYHAL_GPIO_DIR_OUTPUT,      // Direction
				CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
				true);				        // InitialValue

    /* ADD CODE to configure the YELLOW LED as an output */
	cyhal_gpio_init(
				PIN_LED_YELLOW,                // Pin
				CYHAL_GPIO_DIR_OUTPUT,      // Direction
				CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
				true);				        // InitialValue

    /* ADD CODE to configure the BLUE LED as an output */
	cyhal_gpio_init(
				PIN_LED_BLUE,                // Pin
				CYHAL_GPIO_DIR_OUTPUT,      // Direction
				CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
				true);				        // InitialValue
}



