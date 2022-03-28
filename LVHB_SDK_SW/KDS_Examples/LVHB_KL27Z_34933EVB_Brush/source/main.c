/*
 * Copyright (c) 2013 - 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "stdio.h"

#include "lvhb/lvhb.h"
#include "aml/gpio_aml.h"
#include "aml/wait_aml/wait_aml.h"

/* On LVHB action complete event handler. */
void LVHB_OnActionComplete(lvhb_drv_config_t* const drvConfig)
{
    AML_UNUSED(drvConfig);
}

/**
 * This function uses LVHB_RotateProportional method of LVHB driver.
 * Duty of H-Bridge interface 1 is increasing to reach 100%. Then duty is decreased to 0%.
 *
 * The function returns FALSE when an error occurred. Otherwise TRUE is returned.
 */
static bool RotateProportional(lvhb_drv_config_t* const drvConfig)
{
    status_t error;

    /* Change regularly PWM duty to reach 100% value. */
    for (uint8_t pwmDuty = 0; pwmDuty <= 100; pwmDuty++)
    {
        if ((error = LVHB_RotateProportional(drvConfig, pwmDuty, lvhbBridge1)) != kStatus_Success)
        {
            printf("\tReturned error code %u\r\n", (unsigned int) error);
            return false;
        }

        /* Wait. */
        WAIT_AML_WaitMs(100);
    }

    /* Change regularly PWM duty to decrease its value to zero. */
    for (int8_t pwmDuty = 100; pwmDuty >= 0; pwmDuty--)
    {
        if ((error = LVHB_RotateProportional(drvConfig, (uint8_t)pwmDuty, lvhbBridge1)) != kStatus_Success)
        {
            printf("\tReturned error code %u\r\n", (unsigned int) error);
            return false;
        }

        /* Wait. */
        WAIT_AML_WaitMs(100);
    }

    printf("\tResult: OK\r\n");

    return true;
}

/**
 * This function uses LVHB_RotateFull method of LVHBridge driver.
 * It runs motor connected to H-Bridge interface 2. Motor stops. Then it runs
 * at the highest speed. Finally, it stops again.
 *
 * The function returns FALSE when an error occurred. Otherwise TRUE is returned.
 */
static bool RotateFull(lvhb_drv_config_t* const drvConfig)
{
    status_t error;

    /* Stop motor. */
    error = LVHB_RotateFull(drvConfig, lvhbOutputOff, lvhbBridge2);
    if (error != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    /* Run motor. */
    error = LVHB_RotateFull(drvConfig, lvhbOutputOn, lvhbBridge2);
    if (error != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    /* Stop motor. */
    error = LVHB_RotateFull(drvConfig, lvhbOutputOff, lvhbBridge2);
    if (error != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tResult: OK\r\n");
    return true;
}

/**
 * This function sets output of specified H-Bridge interface.
 * H-Bridge interface 1 allows to set speed of motor (method
 * RotateProportional). H-Bridge interface 2 enables to set
 * only on/off states (method RotateFull).
 *
 * Parameter drvConfig - Pointer to driver instance configuration.
 * Parameter rotate    - TRUE for rotation, FALSE for stop.
 * Parameter bridge    - Selection of H-Bridge interface.
 *
 * The function returns FALSE when an error occurred. Otherwise TRUE is returned.
 */
static bool Rotate(lvhb_drv_config_t* const drvConfig, bool rotate, lvhb_bridge_t bridge)
{
    status_t error;

    if (bridge == lvhbBridge1)
    {
        /* Set output of H-Bridge interface 1. */
        error = LVHB_RotateProportional(drvConfig, rotate ? 100 : 0, lvhbBridge1);
    }
    else
    {
        /* Set output of H-Bridge interface 2. */
        error = LVHB_RotateFull(drvConfig, (lvhb_output_state_t) rotate, lvhbBridge2);
    }

    if (error != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    return true;
}

/**
 * This function uses LVHB_RotateProportional and LVHB_SetTriState methods
 * of LVHB driver. It runs motor in forward direction using 100% PWM duty.
 * Then LVHB_SetTriState method is called. Motor should slowly decrease its
 * speed to zero. Brake of motor (LVHB_RotateProportional with 0% duty) follows.
 *
 * The function returns FALSE when an error occurred. Otherwise TRUE is returned.
 */
static bool SetTriState(lvhb_drv_config_t* const drvConfig, lvhb_bridge_t bridge)
{
    status_t error;

    printf("\tRun motor\r\n");
    if (!Rotate(drvConfig, true, bridge))
    {
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tSet output to tri-state (freewheel)\r\n");
    if ((error = LVHB_SetTriState(drvConfig, bridge)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(4);

    printf("\tRun motor\r\n");
    if (!Rotate(drvConfig, true, bridge))
    {
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tSet output to LOW (brake)\r\n");
    if (!Rotate(drvConfig, false, bridge))
    {
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(4);

    printf("\tResult: OK\r\n");
    return true;
}

/**
 * This function contains several test cases to show usage of the LVHBridge driver
 * methods.
 *
 * The function returns FALSE when an error occurred.
 */
static bool StartApplication(lvhb_drv_config_t* const drvConfig)
{

    printf("\r\n1. Test method SetTriState using H-Bridge interface 1\r\n");
    if (!SetTriState(drvConfig, lvhbBridge1))
    {
        return false;
    }


    printf("\r\n2. Test method SetTriState using H-Bridge interface 2\r\n");
    if (!SetTriState(drvConfig, lvhbBridge2))
    {
        return false;
    }

    printf("\r\n3. Test method RotateProportional, RotateFull and SetDirection\r\n");
    while (true)
    {

        /* H-Bridge interface 1. */
        if (LVHB_SetDirection(drvConfig, true, lvhbBridge1) != kStatus_Success)
        {
            return false;
        }

        printf("\tUse method RotateProportional in forward direction to control interface 1\r\n");
        if (!RotateProportional(drvConfig))
        {
            return false;
        }

        if (LVHB_SetDirection(drvConfig, false, lvhbBridge1) != kStatus_Success)
        {
            return false;
        }

        printf("\tUse method RotateProportional in reverse direction to control interface 1\r\n");
        if (!RotateProportional(drvConfig))
        {
            return false;
        }

        /* H-Bridge interface 2. */
        if (LVHB_SetDirection(drvConfig, true, lvhbBridge2) != kStatus_Success)
        {
            return false;
        }

        printf("\tUse method RotateFull in forward direction to control interface 2\r\n");
        if (!RotateFull(drvConfig))
        {
            return false;
        }

        if (LVHB_SetDirection(drvConfig, false, lvhbBridge2) != kStatus_Success)
        {
            return false;
        }

        printf("\tUse method RotateFull in reverse direction to control interface 2\r\n");
        if (!RotateFull(drvConfig))
        {
            return false;
        }
    }

    return true;
}

/*!
 * @brief Application entry point.
 */
int main(void) {
    lvhb_drv_config_t drvConfig;

    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Add your code here. */
    /* Fill LVHB driver config. */
    LVHB_GetDefaultConfig(&drvConfig, lvhbDeviceMC34933, lvhbMotorBrushed);

    drvConfig.deviceConfig.brushPwmFrequency = 10000;
    drvConfig.deviceConfig.secondaryBridgeUsed = true;

    /* Timer settings */
    drvConfig.tmrInstance = 0;                                  /* TPM0 */
    drvConfig.tmrLvhbConfig.counterWidth = 16;
    drvConfig.tmrLvhbConfig.prescale = tmrPrescDiv_8;
    drvConfig.tmrLvhbConfig.srcClck_Hz = CLOCK_GetFreq(kCLOCK_McgIrc48MClk);

    /* Pins */
    drvConfig.inputPins[lvhbBridge1] = lvhbPinsGpioPwm;         /* IN1A (GPIO) + IN1B (PWM) */
    drvConfig.inxaPinInstance[lvhbBridge1] = instanceE;         /* IN1A - PTE24/TPM0_CH0 */
    drvConfig.inxaPinIndex[lvhbBridge1] = 24U;
    drvConfig.tmrLvhbConfig.inxbChannelNumber[lvhbBridge1] = 5; /* IN1B - PTC9/TPM0_CH5 */

    drvConfig.inputPins[lvhbBridge2] = lvhbPinsGpio;            /* IN2A (GPIO) + IN2B (GPIO) */
    drvConfig.inxaPinInstance[lvhbBridge2] = instanceE;         /* IN2A - PTE31/TPM0_CH4 */
    drvConfig.inxaPinIndex[lvhbBridge2] = 31U;
    drvConfig.inxbPinInstance[lvhbBridge2] = instanceA;         /* IN2B - PTA5/TPM0_CH2 */
    drvConfig.inxbPinIndex[lvhbBridge2] = 5U;

    GPIO_AML_SetDirection(instanceB, BOARD_LED_RED_GPIO_PIN, gpioDirDigitalOutput);
    GPIO_AML_SetOutput(instanceB, BOARD_LED_RED_GPIO_PIN);

    CLOCK_SetTpmClock(kTPM_SystemClock);

    if ((LVHB_ConfigureGpio(&drvConfig) != kStatus_Success) ||
        (LVHB_ConfigureTimer(&drvConfig, NULL) != kStatus_Success) ||
        (LVHB_Init(&drvConfig) != kStatus_Success))
    {
        /* An error occurred during initialization. */
        GPIO_AML_ClearOutput(instanceB, BOARD_LED_RED_GPIO_PIN);
    }
    else if (!StartApplication(&drvConfig))
    {
        /* An error occurred. */
        GPIO_AML_ClearOutput(instanceB, BOARD_LED_RED_GPIO_PIN);
    }

    for(;;)  /* Infinite loop to avoid leaving the main function */
    {
        __asm("NOP"); /* something to use as a breakpoint stop while looping */
    }
}

