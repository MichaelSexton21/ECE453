################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/aml/wait_aml/wait_aml.c 

OBJS += \
./source/aml/wait_aml/wait_aml.o 

C_DEPS += \
./source/aml/wait_aml/wait_aml.d 


# Each subdirectory must supply rules for building sources it contributes
source/aml/wait_aml/%.o: ../source/aml/wait_aml/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DCPU_MKL27Z64VDA4 -DCPU_MKL27Z64VDA4_cm0plus -I../CMSIS -I../board -I../drivers -I../utilities -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


