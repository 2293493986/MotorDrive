################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.obj: ../Core/Src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/CCS/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 --include_path="D:/CCS_WorkSpace/Project/MT009_OBJECT/MT009" --include_path="D:/CCS_WorkSpace/Project/MT009_OBJECT/include" --include_path="D:/CCS_WorkSpace/Project/MT009_OBJECT/MT009/Core/Inc" --include_path="D:/CCS/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/include" --advice:performance=all -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Core/Src/$(basename $(<F)).d_raw" --obj_directory="Core/Src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


