#ifndef MOTOR_H__
#define MOTOR_H__

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

//Encoder and two motor input streams

#define MOTOR1_ENCODER_A P5_5
#define MOTOR1_ENCODER_B P5_6
#define MOTOR2_ENCODER_A P10_0
#define MOTOR2_ENCODER_B P10_1

#define MOTOR1_INA P10_2
#define MOTOR1_INB P10_3

#define MOTOR2_INA P10_4
#define MOTOR2_INB P10_5

#define MOTOR_OEn P10_6


#endif
