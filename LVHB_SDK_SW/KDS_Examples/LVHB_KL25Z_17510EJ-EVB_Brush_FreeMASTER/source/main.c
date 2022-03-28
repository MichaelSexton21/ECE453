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

#include "lvhb/lvhb.h"
#include "aml/wait_aml/wait_aml.h"
#include "freemaster_driver/freemaster.h"

/* Number of FreeMASTER commands. */
#define COMMANDS_CNT 7

/* IDs of FreeMASTER commands. */
enum
{
    CMD_RUN = 0,
    CMD_BRAKE = 1,
    CMD_SET_TRISTATE = 2,
    CMD_SET_DIR = 3,
    CMD_SET_SPEED = 4,
    CMD_SET_MODE = 5,
    CMD_SET_GATE = 6
};

/* Size of FreeMASTER command's arguments in bytes. */
static const FMSTR_SIZE CMD_ARG_SIZE[] =
{
    8,  /* Run. */
    0,  /* Brake. */
    0,  /* Set tri-state. */
    8,  /* Set direction. */
    4,  /* Set speed. */
    4,  /* Set mode. */
    4   /* Set gate driver. */
};

lvhb_drv_config_t drvConfig; /* LVHB driver configuration. */

/* On LVHB action complete event handler. */
void LVHB_OnActionComplete(lvhb_drv_config_t* const drvConfig)
{
    AML_UNUSED(drvConfig);
}

/*
 * Process FreeMASTER command RotateProportional.
 *
 * @param forward Motor direction. 1 stands for forward direction, 0 reverse.
 * @param speed Motor speed in percent (0..100).
 */
static void ProcessRotateProportional(uint32_t forward, uint32_t speed)
{
    status_t error;

    /* Set direction. */
    error = LVHB_SetDirection(&drvConfig, (bool)forward, lvhbBridge1);
    /* Run motor at desired speed. */
    error |= LVHB_RotateProportional(&drvConfig, (uint8_t)speed, lvhbBridge1);
    /* Acknowledge command. */
    FMSTR_AppCmdAck(error != kStatus_Success);
}

/*
 * Check and eventually process FreeMASTER command.
 */
static void ProcessCmd(void)
{
    status_t error = kStatus_Success;
    FMSTR_APPCMD_CODE cmd;        /* Freemaster application command. */
    FMSTR_APPCMD_PDATA cmdData;   /* Freemaster pointer to application command data. */
    FMSTR_SIZE cmdSize;           /* Size of application command arguments */
    uint32_t *paramsPtr = NULL;   /* Pointer to parameters of a called command. */

    /* Fetch application command code. */
    cmd = FMSTR_GetAppCmd();

    /* if there is any command proceed to read it and take respective action */
    if ((cmd != FMSTR_APPCMDRESULT_NOCMD) & (cmd < COMMANDS_CNT))
    {

        /* Get pointer to application command data */
        cmdData = FMSTR_GetAppCmdData(&cmdSize);

        /* Check size of arguments. */
        if (cmdSize != CMD_ARG_SIZE[cmd])
        {
            /* Wrong number of parameters. */
            FMSTR_AppCmdAck(true);
            return;
        }

        /* Set pointer to arguments. */
        paramsPtr = (uint32_t *)cmdData;

        switch (cmd)
        {
            case CMD_RUN:
                /* RotateProportional has 2 arguments (Forward, Speed) - 8 bytes */
                ProcessRotateProportional(paramsPtr[0], paramsPtr[1]);
                break;

            case CMD_BRAKE:
                /* Brake has 0 arguments */
                error = LVHB_RotateProportional(&drvConfig, 0, lvhbBridge1);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            case CMD_SET_TRISTATE:
                /* SetTriState has 0 argument */
                error = LVHB_SetTriState(&drvConfig, lvhbBridge1);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            case CMD_SET_DIR:
                /* SetDirection has 1 argument (Forward) - 4 bytes */
                /* The same command as the first one with cmd ID = 0. */
                ProcessRotateProportional(paramsPtr[0], paramsPtr[1]);
                break;

            case CMD_SET_SPEED:
                /* SetSpeed has 1 argument (Speed) - 4 bytes */
                error = LVHB_RotateProportional(&drvConfig, (uint8_t)paramsPtr[0], lvhbBridge1);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            case CMD_SET_MODE:
                /* SetMode has 1 arguments (Mode) - 4 bytes */
                error = LVHB_SetMode(&drvConfig, (bool)paramsPtr[0]);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            case CMD_SET_GATE:
                /* SetGateDriver has 1 arguments (OutputHigh) - 4 bytes */
                error = LVHB_SetGateDriver(&drvConfig, (bool)paramsPtr[0]);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            default:
                /* An error occurred. */
                FMSTR_AppCmdAck(true);
                break;
        }
    }
}

/*
 * Main application control loop. This function contains endless loop.
 */
static void RunApp(void)
{
    while (true)
    {
        /* Refresh timeout of FreeMASTER. */
        FMSTR_Poll();

        /* Process command from GUI. */
        ProcessCmd();

        FMSTR_Poll();

        /* Write global variables to PC. */
        FMSTR_Recorder();
    }
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

    if ((LVHB_ConfigureGpio(&drvConfig) == kStatus_Success) &&
        (LVHB_ConfigureTimer(&drvConfig, NULL) == kStatus_Success) &&
        (LVHB_Init(&drvConfig) == kStatus_Success) &&
        (FMSTR_Init() != 0))
    {
        RunApp();
    }

    for(;;)  /* Infinite loop to avoid leaving the main function */
    {
        __asm("NOP"); /* something to use as a breakpoint stop while looping */
    }
}
