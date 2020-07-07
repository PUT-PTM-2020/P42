################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
radio_temperature_project.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
radio_temperature_project.list \


# All Target
all: radio_temperature_project.elf secondary-outputs

# Tool invocations
radio_temperature_project.elf: $(OBJS) $(USER_OBJS) /Users/mikolaj/STM32CubeIDE/workspace_new/radio_temperature_project/STM32F407VGTX_FLASH.ld
	arm-none-eabi-gcc -o "radio_temperature_project.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/Users/mikolaj/STM32CubeIDE/workspace_new/radio_temperature_project/STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="radio_temperature_project.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

radio_temperature_project.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "radio_temperature_project.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
