/*
 * Copyright (c) 2013 - 2017, Freescale Semiconductor, Inc.
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
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
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
#include "fsl_tpm.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "stdio.h"

#include "lvhb/lvhb.h"
#include "aml/wait_aml/wait_aml.h"

/* On LVHB action complete event handler. */
void LVHB_OnActionComplete(lvhb_drv_config_t* const drvConfig)
{
    AML_UNUSED(drvConfig);
}

/**
 * This function uses method LVHB_RotateProportional of the LVHB driver.
 * Duty is increasing to reach 100%. Then duty is decreased to 0%.
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
 * This function uses LVHB_RotateProportional and LVHB_SetTriState methods
 * of LVHB driver. It runs motor in forward direction using 100% PWM duty.
 * Then LVHB_SetTriState method is called. Motor should slowly decrease its
 * speed to zero. Brake of motor (LVHB_RotateProportional with 0% duty) follows.
 *
 * The function returns FALSE when an error occurred. Otherwise TRUE is returned.
 */
static bool SetTriState(lvhb_drv_config_t* const drvConfig)
{
    status_t error;

    printf("\tRun motor with PWM duty set to 100 percent\r\n");
    if ((error = LVHB_RotateProportional(drvConfig, 100, lvhbBridge1)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tSet output to tri-state (freewheel)\r\n");
    if ((error = LVHB_SetTriState(drvConfig, lvhbBridge1)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(4);

    printf("\tRun motor with PWM duty set to 100 percent\r\n");
    if ((error = LVHB_RotateProportional(drvConfig, 100, lvhbBridge1)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tSet output to LOW (brake)\r\n");
    if ((error = LVHB_RotateProportional(drvConfig, 0, lvhbBridge1)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(4);

    printf("\tResult: OK\r\n");
    return true;
}

/**
 * This function uses LVHB_RotateProportional and LVHB_SetMode methods
 * of LVHB driver. It runs motor in forward direction using 100% PWM duty.
 * Then LVHB_SetMode method is called. Outputs of H-Bridge device should be
 * changed to LOW and motor brakes. After a while the mode is set back to
 * normal operational mode.
 *
 * The function returns FALSE when an error occurred. Otherwise TRUE is returned.
 */
static bool SetMode(lvhb_drv_config_t* const drvConfig)
{
    status_t error;

    /* Run motor. */
    printf("\tRun motor with PWM duty set to 100 percent\r\n");
    if ((error = LVHB_RotateProportional(drvConfig, 100, lvhbBridge1)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tSet sleep mode (EN pin to LOW)\r\n");
    if ((error = LVHB_SetMode(drvConfig, false)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tSet normal mode (EN pin to HIGH)\r\n");
    if ((error = LVHB_SetMode(drvConfig, true)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(2);

    printf("\tStop motor\r\n");
    if ((error = LVHB_RotateProportional(drvConfig, 0, lvhbBridge1)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    printf("\tResult: OK\r\n");
    return true;
}

/**
 * This function uses method LVHB_SetGateDriver of the LVHB driver.
 * It controls H-Bridge output GOUT.
 *
 * The function returns FALSE when an error occurred. Otherwise TRUE is returned.
 */
static bool SetGateDriver(lvhb_drv_config_t* const drvConfig)
{
    status_t error;

    printf("\tDefault value of GOUT is LOW\r\n");

    /* Wait. */
    WAIT_AML_WaitSec(4);

    printf("\tSet GOUT to HIGH\r\n");
    if ((error = LVHB_SetGateDriver(drvConfig, true)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
        return false;
    }

    /* Wait. */
    WAIT_AML_WaitSec(4);

    printf("\tSet GOUT back to LOW\r\n");
    if ((error = LVHB_SetGateDriver(drvConfig, false)) != kStatus_Success)
    {
        printf("\tReturned error code %u\r\n", (unsigned int) error);
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
    printf("\r\n1. Test method SetTriState\r\n");
    if (!SetTriState(drvConfig))
    {
        return false;
    }

    printf("\r\n2. Test method SetMode\r\n");
    if (!SetMode(drvConfig))
    {
        return false;
    }

    printf("\r\n3. Test method SetGateDriver\r\n");
    if (!SetGateDriver(drvConfig))
    {
        return false;
    }

    printf("\r\n4. Test method RotateProportional and SetDirection\r\n");
    while (true)
    {
        if (LVHB_SetDirection(drvConfig, true, lvhbBridge1) != kStatus_Success)
        {
            return false;
        }

        printf("\tUse method RotateProportional in forward direction\r\n");
        if (!RotateProportional(drvConfig))
        {
            return false;
        }

        if (LVHB_SetDirection(drvConfig, false, lvhbBridge1) != kStatus_Success)
        {
            return false;
        }

        printf("\tUse method RotateProportional in reverse direction\r\n");
        if (!RotateProportional(drvConfig))
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
    LVHB_GetDefaultConfig(&drvConfig, lvhbDeviceMPC17510, lvhbMotorBrushed);

    drvConfig.deviceConfig.activeMode = true;
    drvConfig.deviceConfig.gateDriverOutputHigh = false;
    drvConfig.deviceConfig.brushPwmFrequency = 5000;

    /* Timer settings */
    drvConfig.tmrInstance = 0;                                  /* TPM0 */
    drvConfig.tmrLvhbConfig.counterWidth = 16;
    drvConfig.tmrLvhbConfig.prescale = tmrPrescDiv_8;
    drvConfig.tmrLvhbConfig.srcClck_Hz = CLOCK_GetFreq(kCLOCK_PllFllSelClk);

    /* Pins */
    drvConfig.inputPins[lvhbBridge1] = lvhbPinsPwmGpio;         /* IN1 (PWM) + IN2 (GPIO) */
    drvConfig.tmrLvhbConfig.inxaChannelNumber[lvhbBridge1] = 4; /* IN1 - PTD4/TPM0_CH4 */
    drvConfig.inxbPinInstance[lvhbBridge1] = instanceA;         /* IN2 - PTA12 */
    drvConfig.inxbPinIndex[lvhbBridge1] = 12U;

    drvConfig.enPinInstance = instanceC;                        /* EN - PTC7 */
    drvConfig.enPinIndex = 7U;
    drvConfig.ginPinInstance = instanceC;                       /* GIN - PTC0 */
    drvConfig.ginPinIndex = 0U;

    CLOCK_SetTpmClock(kTPM_SystemClock);

    if ((LVHB_ConfigureGpio(&drvConfig) != kStatus_Success) ||
        (LVHB_ConfigureTimer(&drvConfig, NULL) != kStatus_Success) ||
        (LVHB_Init(&drvConfig) != kStatus_Success))
    {
        printf("An error occurred during initialization.\r\n");
    }
    else if (!StartApplication(&drvConfig))
    {
        printf("\tAn error occurred\r\n");
    }

    for(;;)  /* Infinite loop to avoid leaving the main function */
    {
        __asm("NOP"); /* something to use as a breakpoint stop while looping */
    }
}