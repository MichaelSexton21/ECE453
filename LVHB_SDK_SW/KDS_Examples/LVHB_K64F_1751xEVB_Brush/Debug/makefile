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
-include source/aml/wait_aml/subdir.mk
-include source/aml/tmr_aml/subdir.mk
-include drivers/subdir.mk
-include board/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
LVHB_K64F_1751xEVB_Brush.hex \

SECONDARY_SIZE += \
LVHB_K64F_1751xEVB_Brush.siz \


# All Target
all: LVHB_K64F_1751xEVB_Brush.elf secondary-outputs

# Tool invocations
LVHB_K64F_1751xEVB_Brush.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto  -g3 -T "../MK64FN1M0xxx12_flash.ld" -Xlinker --gc-sections -Wl,-Map,"LVHB_K64F_1751xEVB_Brush.map" --specs=nano.specs -specs=nosys.specs -Xlinker -z -Xlinker muldefs -o "LVHB_K64F_1751xEVB_Brush.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

LVHB_K64F_1751xEVB_Brush.hex: LVHB_K64F_1751xEVB_Brush.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "LVHB_K64F_1751xEVB_Brush.elf"  "LVHB_K64F_1751xEVB_Brush.hex"
	@echo 'Finished building: $@'
	@echo ' '

LVHB_K64F_1751xEVB_Brush.siz: LVHB_K64F_1751xEVB_Brush.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "LVHB_K64F_1751xEVB_Brush.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) LVHB_K64F_1751xEVB_Brush.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
