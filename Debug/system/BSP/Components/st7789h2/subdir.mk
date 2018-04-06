################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/BSP/Components/st7789h2/st7789h2.c 

OBJS += \
./system/BSP/Components/st7789h2/st7789h2.o 

C_DEPS += \
./system/BSP/Components/st7789h2/st7789h2.d 


# Each subdirectory must supply rules for building sources it contributes
system/BSP/Components/st7789h2/%.o: ../system/BSP/Components/st7789h2/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/BSP/STM32F4xx-Nucleo" -I"../system/BSP/Components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


