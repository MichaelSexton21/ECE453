/*
 * opt3001.c
 *
 *  Created on: Oct 20, 2020
 *      Author: Joe Krachey
 */

#include "at42qt2120.h"


volatile bool ALERT_AT42QT2120_CHANGE = false;

/* CapSense Handler Struct */
cyhal_gpio_callback_data_t capsense_cb_data =
{
    .callback     = cap_sense_handler,
    .callback_arg = (void*)NULL
};

/***********************************************************
* Initialize CapSense Change Interrupt IO pin for
* falling edge interrupts
************************************************************/
void cap_sense_irq_init(void)
{
	/* ADD CODE */

	// Enable the interrupt for the CapSense Change IRQ
	cyhal_gpio_register_callback(
			PIN_CT_IRQ, 		        	// Pin
			&capsense_cb_data);			    // Handler Callback Info

	cyhal_gpio_enable_event(
			PIN_CT_IRQ, 		        	// Pin
			CYHAL_GPIO_IRQ_FALL, 	    // Event
			INT_PRIORITY_CT, 	        // Priority
			true);			            // Enable Event
}

/*****************************************************
* Function Name: cap_sense_io_init
******************************************************
* Summary:
* Initializes the IO pin used to control AT42
* change pin
*
* Parameters:
*  void
*
* Return:
*
*
*****************************************************/
void cap_sense_io_init(void)
{
    /* ADD CODE to configure CapSense Change Pin as an input */
	cyhal_gpio_init(
			PIN_CT_IRQ,                       // Pin
			CYHAL_GPIO_DIR_INPUT,       // Direction
			CYHAL_GPIO_DRIVE_NONE,    // Drive Mode
			true);				        // InitialValue
}



/*******************************************************************************
* Function Name: cap_sense_handler
********************************************************************************
* Summary:
* Handler for the CT Change line from the CapSense controller
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void cap_sense_handler(void* handler_arg, cyhal_gpio_event_t event)
{
	ALERT_AT42QT2120_CHANGE = true;
}

/** Write a register on the AT42QT2120
 *
 * @param reg The reg address to read
 * @param value The value to be written
 *
 */
void AT42QT2120_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t write_buffer[AT42QT2120_WRITE_PACKET_SIZE];

	/* ADD CODE: Initialize the write_buffer with the data
     * that will sent to the AT42QT2120 */
	write_buffer[0] = reg;
	write_buffer[1] = value;

	/* ADD CODE */
	/* Use cyhal_i2c_master_write to write the required data to the device. */
	cyhal_i2c_master_write(
							&i2c_master_obj, 	// I2C Object
							AT42QT2120_SUBORDINATE_ADDR,	                // I2C Address
							write_buffer, 					// Array of data to write
							AT42QT2120_WRITE_PACKET_SIZE, 		// Number of bytes to write
							0, 					// Block until completed
							1);				    // Generate Stop Condition

}

/** Read a register on the AT42QT2120
 *
 * @param reg The reg address to read
 *
 */
uint8_t AT42QT2120_read_reg(uint8_t reg)
{
	uint8_t write_buffer[1];
	uint8_t read_buffer[1];

	write_buffer[0] = reg;

	/* ADD CODE */
	/* Use cyhal_i2c_master_write to write the required data to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	if ( CY_RSLT_SUCCESS == cyhal_i2c_master_write(
							                        &i2c_master_obj,    // I2C Object
													AT42QT2120_SUBORDINATE_ADDR,	            // I2C Address
							                        write_buffer, 		// Array of data to write
							                        1, 				    // Number of bytes to write
							                        0, 					// Block until completed
							                        0))					// Do NOT generate Stop Condition
	{

		/* ADD CODE */
		/* Use cyhal_i2c_master_read to read the required data from the device. */
		// The register address has already been set in the write above, so read a single byte
		// of data.
		if ( CY_RSLT_SUCCESS == cyhal_i2c_master_read(
								                    &i2c_master_obj, 	// I2C Object
													AT42QT2120_SUBORDINATE_ADDR,	                // I2C Address
								                    read_buffer, 					// Read Buffer
								                    1 , 			    // Number of bytes to read
								                    0, 					// Block until completed
								                    1)) 				// Generate Stop Condition
		{
			return read_buffer[0];
		}
		else
		{
		     /* Disable all interrupts. */
		    __disable_irq();

		    CY_ASSERT(0);

		    while(1){};
		}
	}
	else
	{
	     /* Disable all interrupts. */
	    __disable_irq();

	    CY_ASSERT(0);

	    while(1){};
	}

	return 0xFF; // Should never get here
}

/** Read Chip ID Register
 *
 *
 *
 */
uint8_t AT42QT2120_read_chip_id(void)
{
    return AT42QT2120_read_reg(AT42QT2120_CHIP_ID_REG);
}

/** Read Chip Status Low Register
 *
 *
 *
 */
uint8_t AT42QT2120_read_key_status_lo(void)
{
    return AT42QT2120_read_reg(AT42QT2120_KEY_STATUS_LO_REG);
}

/** Read Chip Status Hi Register
 *
 *
 *
 */
uint8_t AT42QT2120_read_key_status_hi(void)
{
    return AT42QT2120_read_reg(AT42QT2120_KEY_STATUS_HI_REG);
}

/**
 *
 *
 *
 */
uint8_t AT42QT2120_read_detection_status(void)
{
    return AT42QT2120_read_reg(AT42QT2120_DET_STATUS_REG);
}

/**
 *
 *
 *
 */
uint8_t AT42QT2120_read_slider_status(void)
{
    return AT42QT2120_read_reg(AT42QT2120_KEY_SLIDER_POS_REG);
}

uint8_t AT42QT2120_read_buttons(void)
{
	uint8_t junk;
    junk = AT42QT2120_read_key_status_hi();
    junk = AT42QT2120_read_detection_status();
    junk = AT42QT2120_read_slider_status();
    return AT42QT2120_read_key_status_lo();
}


void AT42QT2120_init(void)
{
	cap_sense_io_init();
	cap_sense_irq_init();
}




