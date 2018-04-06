################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../qpcpp/ports/arm-cm/qxk/gnu/qxk_port.S 

OBJS += \
./qpcpp/ports/arm-cm/qxk/gnu/qxk_port.o 

S_UPPER_DEPS += \
./qpcpp/ports/arm-cm/qxk/gnu/qxk_port.d 


# Each subdirectory must supply rules for building sources it contributes
qpcpp/ports/arm-cm/qxk/gnu/%.o: ../qpcpp/ports/arm-cm/qxk/gnu/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -x assembler-with-cpp -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../framework/include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


