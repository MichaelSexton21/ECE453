/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Empty PSoC6 Application
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* (c) 2019-2021, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

#include "main.h"

#define ENABLE_I2C 0

int main(void)
{
	cy_rslt_t result;
	uint8_t button_status;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    sw2_init();
    leds_init();
    console_init();
#if ENABLE_I2C
    i2c_init();
    AT42QT2120_init();
#endif

    __enable_irq();

#if ENABLE_I2C
    button_status = AT42QT2120_read_buttons();
#endif

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("\r\n");
    printf("*****************************\r\n");
    printf("* ECE453 Lab 3 Solutions\r\n");
    printf("*****************************\r\n");
    for (;;)
    {
    	if(ALERT_CONSOLE_RX)
		{
			ALERT_CONSOLE_RX = false;
			printf("\r\nRx --> %s\r\n", pcInputString);
			cInputIndex = 0;
			memset(pcInputString,0,DEBUG_MESSAGE_MAX_LEN);
		}
#if ENABLE_I2C
    	if(ALERT_AT42QT2120_CHANGE)
		{
			ALERT_AT42QT2120_CHANGE = false;

			button_status = AT42QT2120_read_buttons();

			if(button_status & 0x01)
			{
				cyhal_gpio_write(PIN_LED_BLUE, 0); // ON
			}
			else
			{
				cyhal_gpio_write(PIN_LED_BLUE, 1); // OFF
			}

			if(button_status & 0x04)
			{
				cyhal_gpio_write(PIN_LED_YELLOW, 0); // ON
			}
			else
			{
				cyhal_gpio_write(PIN_LED_YELLOW, 1); // OFF
			}

			if(button_status & 0x02)
			{
				cyhal_gpio_write(PIN_LED_GREEN, 0); // ON
			}
			else
			{
				cyhal_gpio_write(PIN_LED_GREEN, 1); // OFF
			}

			if(button_status & 0x08)
			{
				cyhal_gpio_write(PIN_LED_RED, 0); // ON
			}
			else
			{
				cyhal_gpio_write(PIN_LED_RED, 1); // OFF
			}
		}
#endif
    }
}



/* [] END OF FILE */
