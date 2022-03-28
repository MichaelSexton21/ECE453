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

lvhb_drv_config_t drvConfig; /* LVHB driver configuration structure. */

/* Timer interrupt handler. It clears an interrupt flag
 * and call on counter restart handler from LVHB driver.
 */
void TPM0_IRQHandler(void)
{
    uint16_t status;
    uint16_t enIntMask;

    status = TMR_AML_GetStatusFlags(drvConfig.tmrInstance);
    enIntMask = TMR_AML_GetEnabledInterrupts(drvConfig.tmrInstance);

    if (((tmrStsTimeOverflow & status) != 0U) &&
            ((tmrIntTimeOverflow & enIntMask) != 0U))
    {
        TMR_AML_ClearStatusFlags(drvConfig.tmrInstance, tmrStsTimeOverflow);

        /* Invoke OnCounterRestart LVHB event */
        LVHB_OnCounterRestart(&drvConfig);
    }
}

/*
 * On LVHB action complete event handler.
 */
void LVHB_OnActionComplete(lvhb_drv_config_t* const drvConfig)
{
    AML_UNUSED(drvConfig);
}

/*
 * Wait for completion of motor movement.
 */
static void WaitForCompletion(lvhb_drv_config_t* const drvConfig)
{
    while (LVHB_GetMotorStatus(drvConfig) == lvhbStatusRunning);
}

/*
 * Quick demo demonstrates how to use driver in a common way.
 * It demonstrates how to control motor in full-step mode
 * and how to change motor speed.
 *
 * Returns FALSE if an error occurred, otherwise TRUE is returned.
 */
static bool BasicDemo(lvhb_drv_config_t* const drvConfig, bool forwardDir)
{
    /* Set the full-stepping speed. */
    if (LVHB_SetFullStepSpeed(drvConfig, 50) != kStatus_Success)
    {
        return false;
    }

    if (LVHB_SetFullStepAcceleration(drvConfig, 0) != kStatus_Success)
    {
        return false;
    }

    /* Run motor in full-step mode (180 degrees). */
    if (LVHB_MoveSteps(drvConfig, forwardDir, 100) != kStatus_Success)
    {
        return false;
    }

    WaitForCompletion(drvConfig);

    WAIT_AML_WaitSec(1);

    return true;
}

/*
 * This function demonstrates how to control motor using continual
 * mode, how to get full-step position, hold motor position
 * (coils are powered) and disable motor (position is not hold).
 *
 * Returns FALSE if an error occurred, otherwise TRUE is returned.
 */
static bool AdvancedDemo(lvhb_drv_config_t* const drvConfig)
{
    bool forward = true;  /* Motor direction. */

    if (LVHB_ResetFullStepPosition(drvConfig) != kStatus_Success)
    {
        return false;
    }

    /* Run motor in full-step mode using various speed. */
    for (uint16_t speed = 25; speed <= 100; speed += 25)
    {
        /* Set new full-step speed. Note that maximum speed also depends on used
         * stepper motor. */
        if (LVHB_SetFullStepSpeed(drvConfig, speed) != kStatus_Success)
        {
            continue;
        }

        /* Start continual movement. */
        if (LVHB_MoveContinual(drvConfig, forward) != kStatus_Success)
        {
            return false;
        }

        if (forward)
        {
            /* Wait until motor executes at least 100 full-steps in forward direction. */
            while (LVHB_GetFullStepPosition(drvConfig) < 100);
        }
        else
        {
            /* Wait until motor executes at least 100 full-steps in reversed direction. */
            while (LVHB_GetFullStepPosition(drvConfig) > 0);
        }

        /* Stop continual movement. */
        if (LVHB_StopContinualMovement(drvConfig) != kStatus_Success)
        {
            return false;
        }

        /* Wait until motor stops. */
        WaitForCompletion(drvConfig);

        /* Change direction. */
        forward = !forward;
    }

    /* Reset full-step position. */
    if (LVHB_ResetFullStepPosition(drvConfig) != kStatus_Success)
    {
        return false;
    }

    /* Set H-Bridge outputs to LOW.*/
    if (LVHB_DisableMotor(drvConfig) != kStatus_Success)
    {
        return false;
    }

    /* Wait for a while to see change of H-Bridge outputs. */
    WAIT_AML_WaitSec(1);

    /* No error. */
    return true;
}

/**
 * There are several functions, which are described below.
 * Rotor align is performed after initialization. This function
 * executes 4 full step to set rotor to a full-step position.
 * Then BasicDemo and AdvancedDemo functions are called in a loop.
 *
 * Returns FALSE if an error occurred.
 */
static bool StartApplication(lvhb_drv_config_t* const drvConfig)
{
    bool forwardDir = true;     /* Motor direction. */

    /* Align motor to full-step position. */
    if (LVHB_AlignRotor(drvConfig) != kStatus_Success)
    {
        return false;
    }

    WaitForCompletion(drvConfig);

    while (true)
    {
        /* Basic stepper motor control. */
        if (!BasicDemo(drvConfig, forwardDir))
        {
            return false;
        }

        /* Advanced motor control demo, which uses continual mode. */
        if (!AdvancedDemo(drvConfig))
        {
            return false;
        }

        /* Reverse direction. */
        forwardDir = !forwardDir;
    }

    return true;
}

/*!
 * @brief Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Add your code here. */
    /* Fill LVHB driver config. */
    LVHB_GetDefaultConfig(&drvConfig, lvhbDeviceMC34933, lvhbMotorStepper);

    /* Timer settings */
    drvConfig.tmrInstance = 0;                                  /* TPM0 */
    drvConfig.tmrLvhbConfig.counterWidth = 16;
    drvConfig.tmrLvhbConfig.prescale = tmrPrescDiv_8;
    drvConfig.tmrLvhbConfig.srcClck_Hz = CLOCK_GetFreq(kCLOCK_PllFllSelClk);

    /* Pins */
    drvConfig.inputPins[lvhbBridge1] = lvhbPinsGpio;            /* IN1A (GPIO) + IN1B (GPIO) */
    drvConfig.inputPins[lvhbBridge2] = lvhbPinsGpio;            /* IN2A (GPIO) + IN2B (GPIO) */
    drvConfig.inxaPinInstance[lvhbBridge1] = instanceD;         /* IN1A - PTD4/TPM0_CH4 */
    drvConfig.inxaPinIndex[lvhbBridge1] = 4U;
    drvConfig.inxbPinInstance[lvhbBridge1] = instanceA;         /* IN1B - PTA12 */
    drvConfig.inxbPinIndex[lvhbBridge1] = 12U;
    drvConfig.inxaPinInstance[lvhbBridge2] = instanceA;         /* IN2A - PTA4/TPM0_CH1 */
    drvConfig.inxaPinIndex[lvhbBridge2] = 4U;
    drvConfig.inxbPinInstance[lvhbBridge2] = instanceA;         /* IN2B - PTA5/TPM0_CH2 */
    drvConfig.inxbPinIndex[lvhbBridge2] = 5U;

    CLOCK_SetTpmClock(kTPM_SystemClock);

    if ((LVHB_ConfigureGpio(&drvConfig) != kStatus_Success) ||
        (LVHB_ConfigureTimer(&drvConfig, NULL) != kStatus_Success) ||
        (LVHB_Init(&drvConfig) != kStatus_Success))
    {
        printf("An error occurred during initialization.\r\n");
    }
    else
    {
        /* Enable interrupts for TPM0. */
        EnableIRQ(TPM0_IRQn);

        if (!StartApplication(&drvConfig))
        {
            printf("\tAn error occurred\r\n");
        }
    }

    for(;;)  /* Infinite loop to avoid leaving the main function */
    {
        __asm("NOP"); /* something to use as a breakpoint stop while looping */
    }
}
