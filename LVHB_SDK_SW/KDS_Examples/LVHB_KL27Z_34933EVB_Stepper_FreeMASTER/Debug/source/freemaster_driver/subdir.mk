################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/freemaster_driver/freemaster_Kxx.c \
../source/freemaster_driver/freemaster_appcmd.c \
../source/freemaster_driver/freemaster_bdm.c \
../source/freemaster_driver/freemaster_can.c \
../source/freemaster_driver/freemaster_lin.c \
../source/freemaster_driver/freemaster_pipes.c \
../source/freemaster_driver/freemaster_protocol.c \
../source/freemaster_driver/freemaster_rec.c \
../source/freemaster_driver/freemaster_scope.c \
../source/freemaster_driver/freemaster_serial.c \
../source/freemaster_driver/freemaster_sfio.c \
../source/freemaster_driver/freemaster_tsa.c 

OBJS += \
./source/freemaster_driver/freemaster_Kxx.o \
./source/freemaster_driver/freemaster_appcmd.o \
./source/freemaster_driver/freemaster_bdm.o \
./source/freemaster_driver/freemaster_can.o \
./source/freemaster_driver/freemaster_lin.o \
./source/freemaster_driver/freemaster_pipes.o \
./source/freemaster_driver/freemaster_protocol.o \
./source/freemaster_driver/freemaster_rec.o \
./source/freemaster_driver/freemaster_scope.o \
./source/freemaster_driver/freemaster_serial.o \
./source/freemaster_driver/freemaster_sfio.o \
./source/freemaster_driver/freemaster_tsa.o 

C_DEPS += \
./source/freemaster_driver/freemaster_Kxx.d \
./source/freemaster_driver/freemaster_appcmd.d \
./source/freemaster_driver/freemaster_bdm.d \
./source/freemaster_driver/freemaster_can.d \
./source/freemaster_driver/freemaster_lin.d \
./source/freemaster_driver/freemaster_pipes.d \
./source/freemaster_driver/freemaster_protocol.d \
./source/freemaster_driver/freemaster_rec.d \
./source/freemaster_driver/freemaster_scope.d \
./source/freemaster_driver/freemaster_serial.d \
./source/freemaster_driver/freemaster_sfio.d \
./source/freemaster_driver/freemaster_tsa.d 


# Each subdirectory must supply rules for building sources it contributes
source/freemaster_driver/%.o: ../source/freemaster_driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DCPU_MKL27Z64VDA4 -DCPU_MKL27Z64VDA4_cm0plus -I../CMSIS -I../board -I../drivers -I../utilities -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


