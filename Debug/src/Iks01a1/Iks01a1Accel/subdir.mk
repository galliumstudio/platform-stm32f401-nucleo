################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Iks01a1/Iks01a1Accel/Iks01a1Accel.cpp 

OBJS += \
./src/Iks01a1/Iks01a1Accel/Iks01a1Accel.o 

CPP_DEPS += \
./src/Iks01a1/Iks01a1Accel/Iks01a1Accel.d 


# Each subdirectory must supply rules for building sources it contributes
src/Iks01a1/Iks01a1Accel/%.o: ../src/Iks01a1/Iks01a1Accel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/BSP/STM32F4xx-Nucleo" -I"../system/BSP/Components" -I../qpcpp/ports/arm-cm/qxk/gnu -I../qpcpp/include -I../qpcpp/src -I"../framework/include" -I"../src/Console" -I"../src/Console/CmdInput" -I"../src/Console/CmdParser" -I"../src/System" -I"../src/WifiSt" -I"../src/Template/CompositeAct" -I"../src/Template/CompositeAct/CompositeReg" -I"../src/Template/SimpleAct" -I"../src/Template/SimpleReg" -I"../src/UartAct" -I"../src/UartAct/UartIn" -I"../src/UartAct/UartOut" -std=c++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -std=c++14 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


