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

#include "lvhb/lvhb.h"
#include "freemaster_driver/freemaster.h"

/* Number of FreeMASTER commands. */
#define COMMANDS_CNT 8

/* IDs of FreeMASTER commands. */
enum {
    CMD_MOVE_STEPS = 0,
    CMD_MOVE_MICROSTEPS = 1,
    CMD_RESET_POS = 2,
    CMD_DISABLE = 3,
    CMD_ALIGN = 4,
    CMD_MOVE_CONT = 5,
    CMD_MOVE_MICRO_CONT = 6,
    CMD_STOP_CONT = 7
};

/* Size of FreeMASTER command's arguments in bytes. */
static const FMSTR_SIZE CMD_ARG_SIZE[] = {
    12, /* MoveSteps. */
    16, /* MoveMicroSteps. */
    0,  /* ResetPosition. */
    0,  /* DisableMotor. */
    0,  /* AlignRotor. */
    8,  /* MoveContinual. */
    12, /* MoveMicroContinual. */
    0   /* StopContinual. */
};

/* Stepper state. */
typedef struct {
    lvhb_motor_status_t status;  /* Motor state. */
    int32_t position;            /* Motor position. */
} stepper_data_t;

static stepper_data_t stepper;   /* Stepper state used by FreeMASTER. */
lvhb_drv_config_t drvConfig;     /* LVHB driver configuration structure. */

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

        /* Invoke OnCounterRestart MVHB event */
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

/**
 * Process FreeMASTER command MoveSteps.
 *
 * @param forward Motor direction. 1 for forward direction, 0 reverse.
 * @param steps Number of steps to be executed.
 * @param speed Motor speed in number of full-steps per second.
 */
void ProcessMoveSteps(uint32_t forward, uint32_t steps, uint32_t speed)
{
    status_t error;

    /* Change speed according to value set in FreeMASTER GUI. */
    error = LVHB_SetFullStepSpeed(&drvConfig, speed);
    /* Run motor. */
    error |= LVHB_MoveSteps(&drvConfig, (bool)forward, steps);

    FMSTR_AppCmdAck(error != kStatus_Success);
}

/**
 * Process FreeMASTER command MoveContinual.
 *
 * @param forward Motor direction. 1 for forward direction, 0 reverse.
 * @param speed Motor speed in number of full-steps per second.
 */
void ProcessMoveContinual(uint32_t forward, uint32_t speed)
{
    status_t error;

    /* Change speed according to value set in FreeMASTER GUI. */
    error = LVHB_SetFullStepSpeed(&drvConfig, speed);
    /* Run motor. */
    error |= LVHB_MoveContinual(&drvConfig, (bool)forward);

    FMSTR_AppCmdAck(error != kStatus_Success);
}

/**
 * Process FreeMASTER command MoveMicroSteps.
 *
 * @param forward Motor direction. 1 for forward direction, 0 reverse.
 * @param steps Number of steps to be executed.
 * @param speed Motor speed in number of micro-steps per second.
 * @param stepSize Size of micro-step in number of micro-steps per full-step.
 */
void ProcessMoveMicroSteps(uint32_t forward, uint32_t steps, uint32_t speed, uint32_t stepSize)
{
    status_t error;

    /* Change speed according to value set in FreeMASTER GUI. */
    error = LVHB_SetMicroStepSpeed(&drvConfig, speed);
    error |= LVHB_SetMicroStepSize(&drvConfig, LVHB_MICROSTEP_CNT_MAX/stepSize);

    /* Run motor. */
    error |= LVHB_MoveMicroSteps(&drvConfig, (bool)forward, steps);

    FMSTR_AppCmdAck(error != kStatus_Success);
}

/**
 * Process FreeMASTER command ProcessMoveMicroContinual.
 *
 * @param forward Motor direction. 1 for forward direction, 0 reverse.
 * @param speed Motor speed in number of micro-steps per second.
 * @param stepSize Size of micro-step in number of micro-steps per full-step.
 */
void ProcessMoveMicroContinual(uint32_t forward, uint32_t speed, uint32_t stepSize)
{
    status_t error;

    /* Change speed according to value set in FreeMASTER GUI. */
    error = LVHB_SetMicroStepSpeed(&drvConfig, speed);
    error |= LVHB_SetMicroStepSize(&drvConfig, LVHB_MICROSTEP_CNT_MAX/stepSize);

    /* Run motor. */
    error |= LVHB_MoveMicroContinual(&drvConfig, (bool)forward);

    FMSTR_AppCmdAck(error != kStatus_Success);
}

/*
 * Check and eventually process FreeMASTER command.
 */
static void ProcessCmd()
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
            case CMD_MOVE_STEPS:
                /* MoveSteps has 3 arguments (Forward, Steps, Speed) - 12 bytes. */
                ProcessMoveSteps(paramsPtr[0], paramsPtr[1], paramsPtr[2]);
                break;

            case CMD_MOVE_MICROSTEPS:
                /* MoveMicroSteps has 4 arguments (Forward, Steps, Speed, StepSize) - 16 bytes. */
                ProcessMoveMicroSteps(paramsPtr[0], paramsPtr[1], paramsPtr[2], paramsPtr[3]);
                break;

            case CMD_RESET_POS:
                /* ResetPostion has 0 argument. */
                error = LVHB_ResetFullStepPosition(&drvConfig);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            case CMD_DISABLE:
                /* DisableMotor has 0 argument. */
                error = LVHB_DisableMotor(&drvConfig);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            case CMD_ALIGN:
                /* AlignRotor has 0 argument. */
                error = LVHB_AlignRotor(&drvConfig);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            case CMD_MOVE_CONT:
                /* MoveContinual has 2 arguments (Forward, Speed) - 8 bytes. */
                ProcessMoveContinual(paramsPtr[0], paramsPtr[1]);
                break;

            case CMD_MOVE_MICRO_CONT:
                /* MoveMicroContinual has 3 arguments (Forward, Steps, Speed) - 12 bytes. */
                ProcessMoveMicroContinual(paramsPtr[0], paramsPtr[1], paramsPtr[2]);
                break;

            case CMD_STOP_CONT:
                /* StopContinual has 0 argument */
                error = LVHB_StopContinualMovement(&drvConfig);
                FMSTR_AppCmdAck(error != kStatus_Success);
                break;

            default:
                /* An error occurred. */
                FMSTR_AppCmdAck(true);
                break;
        }
    }
}

/**
 * Check motor state and update variables shared with FreeMASTER application.
 */
static void UpdateVariables()
{
    /* Update motor status. */
    stepper.status = LVHB_GetMotorStatus(&drvConfig);
    /* Update motor position in micro-steps. */
    stepper.position = LVHB_GetMicroStepPosition(&drvConfig);
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

        /* Update stepper motor control GUI. */
        UpdateVariables();

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
    LVHB_GetDefaultConfig(&drvConfig, lvhbDeviceMC34933, lvhbMotorStepper);

    drvConfig.deviceConfig.stepperConfig.fullStepSpeed = 50;
    drvConfig.deviceConfig.stepperConfig.fullStepAcceleration = 0;
    drvConfig.deviceConfig.stepperConfig.microStepPerStep = lvhbMicroStep32;
    drvConfig.deviceConfig.stepperConfig.microStepPwmFrequency = 20000;
    drvConfig.deviceConfig.stepperConfig.microStepSpeed = 50;
    drvConfig.deviceConfig.stepperConfig.microStepAcceleration = 0;

    /* Timer settings */
    drvConfig.tmrInstance = 0;                                  /* TPM0 */
    drvConfig.tmrLvhbConfig.counterWidth = 16;
    drvConfig.tmrLvhbConfig.prescale = tmrPrescDiv_8;
    drvConfig.tmrLvhbConfig.srcClck_Hz = CLOCK_GetFreq(kCLOCK_McgIrc48MClk);

    /* Pins */
    drvConfig.inputPins[lvhbBridge1] = lvhbPinsPwm;             /* IN1A (PWM) + IN1B (PWM) */
    drvConfig.inputPins[lvhbBridge2] = lvhbPinsPwm;             /* IN2A (PWM) + IN2B (PWM) */
    drvConfig.tmrLvhbConfig.inxaChannelNumber[lvhbBridge1] = 0; /* IN1A - PTE24/TPM0_CH0 */
    drvConfig.tmrLvhbConfig.inxbChannelNumber[lvhbBridge1] = 5; /* IN1B - PTC9/TPM0_CH5 */
    drvConfig.tmrLvhbConfig.inxaChannelNumber[lvhbBridge2] = 4; /* IN2A - PTE31/TPM0_CH4 */
    drvConfig.tmrLvhbConfig.inxbChannelNumber[lvhbBridge2] = 2; /* IN2B - PTA5/TPM0_CH2 */

    stepper.position = 0;
    stepper.status = lvhbStatusStop;

    CLOCK_SetTpmClock(kTPM_SystemClock);

    /* Initialize GPIO pins, timer, LVHB driver and FreeMASTER. */
    if ((LVHB_ConfigureTimer(&drvConfig, NULL) == kStatus_Success) &&
        (LVHB_Init(&drvConfig) == kStatus_Success) &&
        (FMSTR_Init() != 0))
    {
        /* Enable interrupts for TPM0. */
        EnableIRQ(TPM0_IRQn);

        RunApp();
    }

    for(;;)  /* Infinite loop to avoid leaving the main function */
    {
        __asm("NOP"); /* something to use as a breakpoint stop while looping */
    }
}

