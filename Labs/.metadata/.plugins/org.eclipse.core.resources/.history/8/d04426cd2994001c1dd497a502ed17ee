/******************************************************************************
* File Name: ble_task.c
*
* Description: This file contains the task that initializes BLE and
*              handles different BLE events.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
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


/*******************************************************************************
 * Include header file
 ******************************************************************************/
#include "ble_task.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cycfg_ble.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "status_led_task.h"
#include "uart_debug.h"
#include "ece453_led_task.h"


/******************************************************************************
 * Macros 
 *****************************************************************************/
#define BLE_INIT_TIMEOUT            (pdMS_TO_TICKS(1000u))
#define USER_BTN_INTERRUPT_PRIORITY (7u)


/*****************************************************************************
 * Function prototype
 ****************************************************************************/
static void ble_init(void);
static void stack_event_handler(uint32 event, void* event_param);
static void bas_event_handler(uint32 event, void* event_param);
static void ble_controller_isr(void);
static void ble_start_advertisement(void);
static void user_button_isr(void *handler_arg, cyhal_gpio_irq_event_t event);
static void ble_software_timer_callback(TimerHandle_t xTimer);
static void bless_isr(void);


/******************************************************************************
 * Global variable
 *****************************************************************************/
/* Variable used for storing connection handle */
cy_stc_ble_conn_handle_t conn_handle;

/* Timer handles used for detecting BLE initialization timeout */
TimerHandle_t timerHandle_bleStartTimeout;

/* Queue handle used for BLE command and data */
QueueHandle_t ble_command_data_q;

/* Variable used for detecting BLE initialization timeout */
bool bleInitTimeout = false;

static cy_stc_ble_gatt_write_param_t *write_req_param;


/*******************************************************************************
* Function Name: task_ble
********************************************************************************
* Summary:
*  Task that configures the BLE, register event callback, and processes the
*  BLE state and events based on the current BLE state and data received over
*  ble_command_data_q queue.
*
* Parameters:
*  void *pvParameters : Task parameter defined during task creation (unused
*
*******************************************************************************/
void task_ble(void* param)
{
    /* Variable used to store the battery level */
    uint8_t battery_level;

    /* Variable used to store the return values of RTOS APIs */
    BaseType_t rtos_api_result;

    /* Variable used to store the return values of BLE APIs */
    cy_en_ble_api_result_t ble_api_result;

    /* Send command to process BLE events */
    ble_command_data_t ble_command_data;

    /* Remove warning for unused parameter */
    (void)param;

    /* Initialize BLE */
    ble_init();
    
    /* Initialize and enable switch interrupt. */
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
    cyhal_gpio_register_callback(CYBSP_USER_BTN, user_button_isr, NULL);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, USER_BTN_INTERRUPT_PRIORITY, true);

    for(;;)
    {
        /* Block until a BLE command has been received over bleQueueHandle */
        rtos_api_result = xQueueReceive(ble_command_data_q, &ble_command_data,
                                        portMAX_DELAY);

        /* Command has been received from ble_command_data_q */
        if(rtos_api_result == pdTRUE)
        {
            switch(ble_command_data.command)
            {
                /* Command to process BLE event */
                case BLE_PROCESS_EVENT:
                {
                    /* Cy_Ble_ProcessEvents() allows BLE stack to process
                     * pending events */
                    Cy_BLE_ProcessEvents();
                    break;
                }
                /* Command to update battery level */
                case BATTERY_LEVEL_UPDATE:
                {
                    battery_level = ble_command_data.data;

                    if(CY_BLE_CONN_STATE_CONNECTED == 
                       Cy_BLE_GetConnectionState(conn_handle))
                    {
                        ble_api_result =  Cy_BLE_BASS_SendNotification(
                                            conn_handle, 
                                            CY_BLE_BATTERY_SERVICE_INDEX,
                                            CY_BLE_BAS_BATTERY_LEVEL, 
                                            sizeof(battery_level),
                                            &battery_level);
                                
                        if((ble_api_result != CY_BLE_SUCCESS) && 
                           (ble_api_result != CY_BLE_ERROR_NTF_DISABLED))
                        {
                            task_print_error("Failed to send notification 0x%X",
                                    ble_api_result);
                        }
                        /* Send command to process BLE events  */
                        ble_command_data_t ble_command_data = 
                        {
                            .command = BLE_PROCESS_EVENT
                        };
                        xQueueSendToFront(ble_command_data_q, &ble_command_data,
                                          0u);
                    }
                    break;
                }
                /* Command to handle the GPIO interrupt */
                case HANDLE_GPIO_INTERRUPT:
                {
                    ble_start_advertisement();
                    break;
                }
                /* Unknown command */
                default:
                {
                    task_print_error("Unknown command 0x%X",
                                    ble_command_data.command);
                    break;
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name: ble_init
********************************************************************************
* Summary:
*  This function initializes the BLE Host and Controller, configures BLE
*  interrupt, and registers BAS and Application Host callbacks.
*
*******************************************************************************/
static void ble_init(void)
{
    cy_en_ble_api_result_t ble_api_result;

    /* BLESS interrupt configuration structure */
    static const cy_stc_sysint_t  bless_irq_cfg =
    {
        /* The BLESS interrupt */
        .intrSrc       = bless_interrupt_IRQn,
        /* The interrupt priority number */
        .intrPriority  = 1u
    };
    
    /* Configure and initialize BLESS interrupt. The interrupt is enabled by
     * BLE Init
     */
    cy_ble_config.hw->blessIsrConfig = &bless_irq_cfg;
    Cy_SysInt_Init(cy_ble_config.hw->blessIsrConfig, bless_isr);

    /* Register the generic event handler */
    Cy_BLE_RegisterEventCallback(stack_event_handler);

    /* Initialize the BLE */
    ble_api_result = Cy_BLE_Init(&cy_ble_config);
    if(ble_api_result != CY_BLE_SUCCESS)
    {
        /* BLE stack initialization failed, check configuration, notify error
         * and halt CPU in debug mode
         */
        task_print_error("Cy_BLE_Init API Error: 0x%X \r\n",
                ble_api_result);
        vTaskSuspend(NULL);
    }

    /* Enable BLE */
    ble_api_result = Cy_BLE_Enable();
    if(ble_api_result != CY_BLE_SUCCESS)
    {
        /* BLE stack initialization failed, check configuration, notify error
         * and halt CPU in debug mode
         */
        task_print_error("Cy_BLE_Enable API Error: 0x%X \r\n",
                ble_api_result);
        vTaskSuspend(NULL);
    }

    /* Enable BLE Low Power Mode (LPM) */
    Cy_BLE_EnableLowPowerMode();

    /* Register the Battery Alert Service callback */
    Cy_BLE_BAS_RegisterAttrCallback(bas_event_handler);

    /* Register the application Host callback */
    Cy_BLE_RegisterAppHostCallback(ble_controller_isr);

    /* Create an RTOS timer */
    timerHandle_bleStartTimeout = xTimerCreate ("BLE Timer", BLE_INIT_TIMEOUT,
                                                pdFALSE, NULL,
                                                ble_software_timer_callback);

    /* Start the timer */
    xTimerStart(timerHandle_bleStartTimeout, 0u);

    /* Process BLE event until stack is on. If the BLE does not start within 
     * 1 second software timer will generate an interrupt to indicate the BLE
     * initialization failure.
     */
    while((Cy_BLE_GetState() != CY_BLE_STATE_ON) && !bleInitTimeout)
    {
        /* Process pending BLE event */
        Cy_BLE_ProcessEvents();
    }

    if(bleInitTimeout)
    {
        task_print_error("BLE initialization failed");
        vTaskSuspend(NULL);
    }
    else
    {
        /* BLE started successfully. Stop the timer */
        xTimerStop(timerHandle_bleStartTimeout, 0u);
    }
}


/*******************************************************************************
* Function Name: bless_isr
********************************************************************************
* Summary:
*  Wrapper function for BLESS interrupt
*
*******************************************************************************/
static void bless_isr(void)
{
    /* Process interrupt events generated by the BLE sub-system */
    Cy_BLE_BlessIsrHandler();
}


/*******************************************************************************
* Function Name: ble_controller_isr
********************************************************************************
* Summary:
*  Call back event function to handle interrupts from BLE Controller
*
*******************************************************************************/
static void ble_controller_isr(void)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* Send command to process BLE events  */
    ble_command_data_t ble_command_data = {.command = BLE_PROCESS_EVENT};
    xQueueSendToFrontFromISR(ble_command_data_q, &ble_command_data,
            &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


/*******************************************************************************
* Function Name: stack_event_handler
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  uint32_t event   :  event from the BLE component
*  void* event_param    :  parameters related to the event
*
*******************************************************************************/
static void stack_event_handler(uint32_t event, void* event_param)
{
    BaseType_t rtos_api_result;
    cy_en_ble_api_result_t ble_api_result;

    static cy_stc_ble_gap_sec_key_info_t keyInfo =
    {
        .localKeysFlag    = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST |
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST |
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST,
        .exchangeKeysFlag = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST |
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST |
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_ENC_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_IRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_CSRK_KEY_DIST,
    };

    switch (event)
    {
        /***********************************************************************
        *                       General Events                                 *
        ***********************************************************************/

        /* This event is received when the BLE stack is started */
        case CY_BLE_EVT_STACK_ON:
        {
            task_print_info("Stack on");

            /* Enter into discoverable mode */
            ble_start_advertisement();

            /* Generates the security keys */
            ble_api_result = Cy_BLE_GAP_GenerateKeys(&keyInfo);
            if(ble_api_result != CY_BLE_SUCCESS)
            {
                task_print_error("Generate Key API 0x%X", ble_api_result);
            }
            break;
        }

        /* This event is received when there is a timeout */
        case CY_BLE_EVT_TIMEOUT:
        {
            task_print_info("Timeout event");
            break;
        }

        /* This event indicates completion of Set LE event mask */
        case CY_BLE_EVT_LE_SET_EVENT_MASK_COMPLETE:
        {
            task_print_info("Complete set LE event mask");
            break;
        }

        /* This event indicates set device address command completed */
        case CY_BLE_EVT_SET_DEVICE_ADDR_COMPLETE:
        {
            task_print_info("Complete set device address complete");
            break;
        }

        /* This event indicates set Tx Power command completed */
        case CY_BLE_EVT_SET_TX_PWR_COMPLETE:
        {
            task_print_info("Set Tx power command complete");
            break;
        }

        /**********************************************************************
        *                       GAP Events                                    *
        **********************************************************************/

        /* This event indicates peripheral device has started/stopped
         *  advertising
         */
        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
        {
            if(CY_BLE_ADV_STATE_ADVERTISING == Cy_BLE_GetAdvertisementState())
            {
                task_print_info("Advertisement started");

                status_led_data_t led_data = { LED_TOGGLE_EN };

                rtos_api_result = xQueueOverwrite(status_led_data_q, &led_data);
                /* Check if the operation has been successful */
                if(rtos_api_result != pdTRUE)
                {
                    task_print_error("Sending data to Status LED queue",
                                     rtos_api_result );
                }
            }
            else
            {
                task_print_info("Advertisement stopped");

                status_led_data_t led_data = { LED_TURN_OFF };
                rtos_api_result = xQueueOverwrite(status_led_data_q,
                                                  &led_data);
                /* Check if the operation has been successful */
                if(rtos_api_result != pdTRUE)
                {
                    task_print_error("Sending data to Status LED queue",
                                     rtos_api_result );
                }
            }
            break;
        }

#if (CY_BLE_CONFIG_ENABLE_LL_PRIVACY) /* Layer Privacy enabled */

        /* This event is triggered instead of 'CY_BLE_EVT_GAP_DEVICE_CONNECTED',
        * if Link Layer Privacy is enabled in component customizer
        */
        case CY_BLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
        {
            task_print_info("GAP device connected ", 0u);
            /* sets the security keys that are to be exchanged with a peer
             * device during key exchange stage of the authentication procedure
             */
            keyInfo.SecKeyParam.bdHandle =
                (*(cy_stc_ble_gap_enhance_conn_complete_param_t *)event_param).bdHandle;

            ble_api_result = Cy_BLE_GAP_SetSecurityKeys(&keyInfo);
            if(ble_api_result != CY_BLE_SUCCESS)
            {
                task_print_error("Cy_BLE_GAP_SetSecurityKeys API 0x%X",
                                (uint32_t)ble_api_result);
            }

            status_led_data_t led_data = { LED_TURN_ON };
            rtos_api_result = xQueueOverwrite(status_led_data_q, &led_data);
            /* Check if the operation has been successful */
            if(rtos_api_result != pdTRUE)
            {
                task_print_error("Sending data to Status LED queue 0x%X",
                                rtos_api_result);
            }
            break;
        }
#else 
        /* This event is generated at the GAP Peripheral end after connection
         * is completed with peer Central device
         */
        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
        {
            task_print_info("GAP device connected", 0u);

            /* sets the security keys that are to be exchanged with a peer
             * device during key exchange stage of the authentication procedure
             */
            keyInfo.SecKeyParam.bdHandle =
                (*(cy_stc_ble_gap_connected_param_t *)event_param).bdHandle;
            
            ble_api_result = Cy_BLE_GAP_SetSecurityKeys(&keyInfo);
            if(ble_api_result != CY_BLE_SUCCESS)
            {
                task_print_error("Cy_BLE_GAP_SetSecurityKeys API 0x%X",
                                (uint32_t)ble_api_result);
            }

            status_led_data_t led_data = { LED_TURN_ON };
            rtos_api_result = xQueueOverwrite(status_led_data_q, &led_data);
            /* Check if the operation has been successful */
            if(rtos_api_result != pdTRUE)
            {
                task_print_error("Sending data to Status LED queue 0x%X",
                                 rtos_api_result);
            }
            break;
        }
#endif /* CY_BLE_CONFIG_ENABLE_LL_PRIVACY */

        /* This event is generated when disconnected from remote device or
         * failed to establish connection
         */
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
        {
            task_print_info("GAP device disconnected");
            status_led_data_t led_data = { LED_TURN_OFF };
            rtos_api_result = xQueueOverwrite(status_led_data_q, &led_data);
            /* Check if the operation has been successful */
            if(rtos_api_result != pdTRUE)
            {
                task_print_error("Sending data to Status LED queue 0x%X",
                                 rtos_api_result);
            }
            break;
        }

        /* This event is generated after connection parameter update is
         * requested from the host to the controller
         */
        case CY_BLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE:
        {
            task_print_info("GAP connection update complete");
            break;
        }

        /* GAP authentication request received */
        case CY_BLE_EVT_GAP_AUTH_REQ:
        {
            task_print_info("GAP authentication request");
            if(cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].security ==
              (CY_BLE_GAP_SEC_MODE_1 | CY_BLE_GAP_SEC_LEVEL_1))
            {
                cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].authErr =
                    CY_BLE_GAP_AUTH_ERROR_PAIRING_NOT_SUPPORTED;
            }

            cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].bdHandle =
                ((cy_stc_ble_gap_auth_info_t *)event_param)->bdHandle;

            /* Send Pairing Response */
            Cy_BLE_GAPP_AuthReqReply(&cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]);
            break;
        }

        /* This event indicates authentication process between two devices has
         * failed */
        case CY_BLE_EVT_GAP_AUTH_FAILED:
        {
            task_print_info("GAP authentication failed : 0x%X", \
                    ((cy_stc_ble_gap_auth_info_t*)event_param)->authErr);
            break;
        }

        /* This event indicates security key generation complete */
        case CY_BLE_EVT_GAP_KEYS_GEN_COMPLETE:
        {
            task_print_info("GAP key generation complete");
            keyInfo.SecKeyParam = (*(cy_stc_ble_gap_sec_key_param_t *)event_param);
            Cy_BLE_GAP_SetIdAddress(&cy_ble_deviceAddress);
            break;
        }

        /* This event indicates SMP has completed pairing feature exchange */
        case CY_BLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO:
        {
            task_print_info("Pairing feature exchange complete");
            break;
        }

        /* This event indicates GAP authentication complete */
        case CY_BLE_EVT_GAP_AUTH_COMPLETE:
        {
            task_print_info("GAP Authentication complete");
            break;
        }

        /* This event indicates encryption is changed for an active connection */
        case CY_BLE_EVT_GAP_ENCRYPT_CHANGE:
        {
            task_print_info("GAP encryption change complete");
            break;
        }

        /***********************************************************************
        *                          GATT Events                                 *
        ***********************************************************************/

        /* This event is generated at the GAP Peripheral end after connection
         * is completed with peer Central device
         */
        case CY_BLE_EVT_GATT_CONNECT_IND:
       {
           conn_handle = *(cy_stc_ble_conn_handle_t *)event_param;
           task_print_info("GATT device connected ");
           break;
       }

        /* This event is generated at the GAP Peripheral end after disconnection */
        case CY_BLE_EVT_GATT_DISCONNECT_IND:
        {
            task_print_info("GATT device disconnected");
            break;
        }

        /* This event is triggered when 'GATT MTU Exchange Request' received
         * from GATT client device
         */
        case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
        {
            task_print_info("GATT MTU Exchange Request");
            break;
        }

        /* This event is triggered when a read received from GATT client device */
		case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
		{
			task_print_info("Read request received  for attribute handle 0x%X",
						   (*(cy_stc_ble_gatts_char_val_read_req_t*)event_param).attrHandle);
			break;
		}

		case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:
		{

			write_req_param = (cy_stc_ble_gatt_write_param_t*)event_param;

			if( CY_BLE_LEDS_GREEN_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
			{
				 task_print_info("GATT Write GREEN: 0x%x", write_req_param->handleValPair.value.val[0]);

                 /* ADD CODE to send the PWM value for the GREEN LED to the ECE453 task.
                  * Use ece453_led_task.h to determine which FreeRTOS queue to send the data to
                  * and what the format of the data being sent is.*/

			}
			else if (CY_BLE_LEDS_RED_CHAR_HANDLE == write_req_param->handleValPair.attrHandle)
			{
				task_print_info("GATT Write RED: 0x%x", write_req_param->handleValPair.value.val[0]);

                 /* ADD CODE to send the PWM value for the RED LED to the ECE453 task.
                  * Use ece453_led_task.h to determine which FreeRTOS queue to send the data to
                  * and what the format of the data being sent is.*/

			}
			break;
		}

        /***********************************************************************
        *                           Other Events                               *
        ***********************************************************************/
        default:
        {
            task_print_info("Other event: %0X", (uint32_t)event);
            break;
        }
    }
}


/*******************************************************************************
* Function Name: bas_event_handler
********************************************************************************
* Summary:
*  This is an event callback function to receive BAS events from the BLE
*  Component.
*
* Parameters:
*  uint32_t event   : event from the BLE component
*  void* event_param    : parameters related to the event
*
*******************************************************************************/
static void bas_event_handler(uint32_t event, void* event_param)
{
    switch(event)
    {
        /* This event is received when the notification for Battery Level
         * Characteristic is enabled */
        case CY_BLE_EVT_BASS_NOTIFICATION_ENABLED:
        {
            task_print_info("BAS notification enabled");
            break;
        }
        /* This event is received when the notification for Battery Level
         * Characteristic is disabled */
        case CY_BLE_EVT_BASS_NOTIFICATION_DISABLED:
        {
            task_print_info("BAS notification disabled");
            break;
        }
        default:
        {
            task_print_info("Other BAS event");
            break;
        }
    }
}

/*******************************************************************************
* Function Name: ble_start_advertisement
********************************************************************************
* Summary:
*  This function starts advertisement.
*
*******************************************************************************/
static void ble_start_advertisement(void)
{
    cy_en_ble_api_result_t ble_api_result;

    if((Cy_BLE_GetAdvertisementState() != CY_BLE_ADV_STATE_ADVERTISING) && \
            (Cy_BLE_GetNumOfActiveConn() < CY_BLE_CONN_COUNT))
    {
        ble_api_result = Cy_BLE_GAPP_StartAdvertisement(
                            CY_BLE_ADVERTISING_FAST,
                            CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);

        if(ble_api_result != CY_BLE_SUCCESS)
        {
            task_print_error("Failed to start advertisement 0x%X",
                            ble_api_result);
        }

        /* Send command to process BLE events  */
        ble_command_data_t ble_command_data = { .command = BLE_PROCESS_EVENT };
        xQueueSendToFront(ble_command_data_q, &ble_command_data, 0u);
    }
}


/*******************************************************************************
* Function Name: user_button_isr
********************************************************************************
* Summary:
*  Interrupt service routine for the port interrupt triggered from USER_BTN.
*
* Parameters:
*  void *handler_arg (unused)
*  cyhal_lptimer_irq_event_t event (unused)
*
*******************************************************************************/
static void user_button_isr(void *handler_arg, cyhal_gpio_irq_event_t event)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* Send command to process BLE events  */
    ble_command_data_t ble_command_data = {.command = HANDLE_GPIO_INTERRUPT};
    xQueueSendToFrontFromISR(ble_command_data_q, &ble_command_data,
                             &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


/*******************************************************************************
* Function Name: ble_software_timer_callback
********************************************************************************
* Summary:
*  This function is called when the BLE software Timer expires
*
* Parameters:
*  TimerHandle_t xTimer :  Current timer value (unused)
*
*******************************************************************************/
static void ble_software_timer_callback(TimerHandle_t xTimer)
{
    /* Remove warning for unused parameter */
    (void)xTimer;

    bleInitTimeout = true;
}


/* [] END OF FILE */
