################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/aml/tmr_aml/ftm_aml.c \
../source/aml/tmr_aml/tpm_aml.c 

OBJS += \
./source/aml/tmr_aml/ftm_aml.o \
./source/aml/tmr_aml/tpm_aml.o 

C_DEPS += \
./source/aml/tmr_aml/ftm_aml.d \
./source/aml/tmr_aml/tpm_aml.d 


# Each subdirectory must supply rules for building sources it contributes
source/aml/tmr_aml/%.o: ../source/aml/tmr_aml/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto  -g3 -D"CPU_MK64FN1M0VDC12" -I../startup -I../board -I../utilities -I../CMSIS -I../drivers -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


