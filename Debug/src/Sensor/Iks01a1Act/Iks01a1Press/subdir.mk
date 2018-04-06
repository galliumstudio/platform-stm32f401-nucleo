################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Sensor/Iks01a1Act/Iks01a1Press/Iks01a1Press.cpp 

OBJS += \
./src/Sensor/Iks01a1Act/Iks01a1Press/Iks01a1Press.o 

CPP_DEPS += \
./src/Sensor/Iks01a1Act/Iks01a1Press/Iks01a1Press.d 


# Each subdirectory must supply rules for building sources it contributes
src/Sensor/Iks01a1Act/Iks01a1Press/%.o: ../src/Sensor/Iks01a1Act/Iks01a1Press/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/BSP/STM32F4xx-Nucleo" -I"../system/BSP/Components" -I../qpcpp/ports/arm-cm/qxk/gnu -I../qpcpp/include -I../qpcpp/src -I"../framework/include" -I"../src/Console" -I"../src/Console/CmdInput" -I"../src/Console/CmdParser" -I"../src/System" -I"../src/GpioInAct" -I"../src/GpioInAct/GpioIn" -I"../src/WifiSt" -I"../src/GpioOutAct" -I"../src/GpioOutAct/GpioOut" -I"../src/Sensor" -I"../src/Sensor/Iks01a1" -I"../src/Sensor/Iks01a1/Iks01a1AccelGyro" -I"../src/Sensor/Iks01a1/Iks01a1HumidTemp" -I"../src/Sensor/Iks01a1/Iks01a1Mag" -I"../src/Sensor/Iks01a1/Iks01a1Press" -I"../src/Sensor/Iks01a1/BSP/X_NUCLEO_IKS01A1" -I"../src/Sensor/Iks01a1/BSP/Components/Common" -I"../src/Sensor/Iks01a1/BSP/Components/hts221" -I"../src/Sensor/Iks01a1/BSP/Components/lis3mdl" -I"../src/Sensor/Iks01a1/BSP/Components/lps22hb" -I"../src/Sensor/Iks01a1/BSP/Components/lps25hb" -I"../src/Sensor/Iks01a1/BSP/Components/lsm303agr" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds0" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds3" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6dsl" -I"../src/Template/CompositeAct" -I"../src/Template/CompositeAct/CompositeReg" -I"../src/Template/SimpleAct" -I"../src/Template/SimpleReg" -I"../src/UartAct" -I"../src/UartAct/UartIn" -I"../src/UartAct/UartOut" -std=c++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -std=c++14 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


