/*******************************************************************************
 * Structure and enumeration
 ******************************************************************************/

/* 1 if user is clicking such dir, 0 if they are not (at time of sample)  */
typedef struct
{
	int left, right, up, down;
}   bb8_motor_data_t;

/*******************************************************************************
 * Extern variable
 ******************************************************************************/
/* Handle for the Queue that contains Status LED data */
extern QueueHandle_t bb8_motor_data_q;


/*******************************************************************************
 * Function prototype
 ******************************************************************************/
void task_bb8_motor(void *param);

#endif


/* [] END OF FILE */