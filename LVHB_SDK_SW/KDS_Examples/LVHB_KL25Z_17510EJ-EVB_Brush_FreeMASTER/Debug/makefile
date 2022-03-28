################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/lvhb/subdir.mk
-include source/freemaster_driver/subdir.mk
-include source/aml/wait_aml/subdir.mk
-include source/aml/tmr_aml/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include board/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.hex \

SECONDARY_SIZE += \
LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.siz \


# All Target
all: LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf secondary-outputs

# Tool invocations
LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -T "../MKL25Z128xxx4_flash.ld" -Xlinker --gc-sections -Wl,-Map,"LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.map" --specs=nano.specs -specs=nosys.specs -Xlinker -z -Xlinker muldefs -o "LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.hex: LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf"  "LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.hex"
	@echo 'Finished building: $@'
	@echo ' '

LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.siz: LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(SECONDARY_FLASH)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS)$(S_UPPER_DEPS) LVHB_KL25Z_17510EJ-EVB_Brush_FreeMASTER.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
