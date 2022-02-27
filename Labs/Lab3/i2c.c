/*
 * i2c.c
 *
 *  Created on: Jan 21, 2022
 *      Author: Joe Krachey
 */

#include "i2c.h"

cyhal_i2c_t i2c_master_obj;

// Define the I2C master configuration structure
cyhal_i2c_cfg_t i2c_master_config =
{
    CYHAL_I2C_MODE_MASTER,
    0, // address is not used for master mode
    I2C_MASTER_FREQUENCY
};

/*******************************************************************************
* Function Name: ece453_init_i2c
********************************************************************************
* Summary:
* Initializes the I2C interface
*
* Parameters:
*  void
*
* Return:
*
*
*******************************************************************************/
void i2c_init(void)
{
    // Initialize I2C master, set the SDA and SCL pins and assign a new clock
    cyhal_i2c_init(&i2c_master_obj, PIN_MCU_SDA, PIN_MCU_SCL, NULL);

    // Configure the I2C resource to be master
    cyhal_i2c_configure(&i2c_master_obj, &i2c_master_config) ;
}
