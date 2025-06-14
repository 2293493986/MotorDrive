################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/CCS/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./Core/Src/adc.obj" \
"./Core/Src/foc.obj" \
"./Core/Src/gpio.obj" \
"./Core/Src/paremeter.obj" \
"./Core/Src/pid.obj" \
"./Core/Src/pwm.obj" \
"./Core/Src/smo.obj" \
"./Core/Src/timer.obj" \
"./Core/Src/uart.obj" \
"./System/F2806x_Adc.obj" \
"./System/F2806x_CodeStartBranch.obj" \
"./System/F2806x_Comp.obj" \
"./System/F2806x_CpuTimers.obj" \
"./System/F2806x_DBGIER.obj" \
"./System/F2806x_DefaultIsr.obj" \
"./System/F2806x_DisInt.obj" \
"./System/F2806x_EPwm.obj" \
"./System/F2806x_GlobalVariableDefs.obj" \
"./System/F2806x_Gpio.obj" \
"./System/F2806x_I2C.obj" \
"./System/F2806x_MemCopy.obj" \
"./System/F2806x_OscComp.obj" \
"./System/F2806x_PieCtrl.obj" \
"./System/F2806x_PieVect.obj" \
"./System/F2806x_Sci.obj" \
"./System/F2806x_Spi.obj" \
"./System/F2806x_SysCtrl.obj" \
"./System/F2806x_usDelay.obj" \
"../F28069.cmd" \
"../F2806x_Headers_nonBIOS.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Core/Src/subdir_vars.mk
-include System/subdir_vars.mk
-include subdir_rules.mk
-include Core/Src/subdir_rules.mk
-include System/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
MT009_Sensorless.out \

EXE_OUTPUTS__QUOTED += \
"MT009_Sensorless.out" \

BIN_OUTPUTS += \
MT009_Sensorless.hex \

BIN_OUTPUTS__QUOTED += \
"MT009_Sensorless.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "MT009_Sensorless.out"

# Tool invocations
MT009_Sensorless.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"D:/CCS/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --advice:performance=all -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"MT009_Sensorless.map" --stack_size=0x300 --warn_sections -i"D:/CCS/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/lib" -i"D:/CCS/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="MT009_Sensorless_linkInfo.xml" --rom_model -o "MT009_Sensorless.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

MT009_Sensorless.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: C2000 Hex Utility'
	"D:/CCS/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/bin/hex2000" --diag_wrap=off -o "MT009_Sensorless.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "Core\Src\adc.obj" "Core\Src\foc.obj" "Core\Src\gpio.obj" "Core\Src\paremeter.obj" "Core\Src\pid.obj" "Core\Src\pwm.obj" "Core\Src\smo.obj" "Core\Src\timer.obj" "Core\Src\uart.obj" "System\F2806x_Adc.obj" "System\F2806x_CodeStartBranch.obj" "System\F2806x_Comp.obj" "System\F2806x_CpuTimers.obj" "System\F2806x_DBGIER.obj" "System\F2806x_DefaultIsr.obj" "System\F2806x_DisInt.obj" "System\F2806x_EPwm.obj" "System\F2806x_GlobalVariableDefs.obj" "System\F2806x_Gpio.obj" "System\F2806x_I2C.obj" "System\F2806x_MemCopy.obj" "System\F2806x_OscComp.obj" "System\F2806x_PieCtrl.obj" "System\F2806x_PieVect.obj" "System\F2806x_Sci.obj" "System\F2806x_Spi.obj" "System\F2806x_SysCtrl.obj" "System\F2806x_usDelay.obj" 
	-$(RM) "main.d" "Core\Src\adc.d" "Core\Src\foc.d" "Core\Src\gpio.d" "Core\Src\paremeter.d" "Core\Src\pid.d" "Core\Src\pwm.d" "Core\Src\smo.d" "Core\Src\timer.d" "Core\Src\uart.d" "System\F2806x_Adc.d" "System\F2806x_Comp.d" "System\F2806x_CpuTimers.d" "System\F2806x_DefaultIsr.d" "System\F2806x_EPwm.d" "System\F2806x_GlobalVariableDefs.d" "System\F2806x_Gpio.d" "System\F2806x_I2C.d" "System\F2806x_MemCopy.d" "System\F2806x_OscComp.d" "System\F2806x_PieCtrl.d" "System\F2806x_PieVect.d" "System\F2806x_Sci.d" "System\F2806x_Spi.d" "System\F2806x_SysCtrl.d" 
	-$(RM) "System\F2806x_CodeStartBranch.d" "System\F2806x_DBGIER.d" "System\F2806x_DisInt.d" "System\F2806x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

