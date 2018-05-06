################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../qpcpp/src/qxk/qxk.cpp \
../qpcpp/src/qxk/qxk_mutex.cpp \
../qpcpp/src/qxk/qxk_sema.cpp \
../qpcpp/src/qxk/qxk_xthr.cpp 

OBJS += \
./qpcpp/src/qxk/qxk.o \
./qpcpp/src/qxk/qxk_mutex.o \
./qpcpp/src/qxk/qxk_sema.o \
./qpcpp/src/qxk/qxk_xthr.o 

CPP_DEPS += \
./qpcpp/src/qxk/qxk.d \
./qpcpp/src/qxk/qxk_mutex.d \
./qpcpp/src/qxk/qxk_sema.d \
./qpcpp/src/qxk/qxk_xthr.d 


# Each subdirectory must supply rules for building sources it contributes
qpcpp/src/qxk/%.o: ../qpcpp/src/qxk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/BSP/STM32F4xx-Nucleo" -I"../system/BSP/Components" -I../qpcpp/ports/arm-cm/qxk/gnu -I../qpcpp/include -I../qpcpp/src -I"../framework/include" -I"../src/Console" -I"../src/Console/CmdInput" -I"../src/Console/CmdParser" -I"../src/System" -I"../src/GpioInAct" -I"../src/GpioInAct/GpioIn" -I"../src/WifiSt" -I"../src/GpioOutAct" -I"../src/GpioOutAct/GpioOut" -I"../src/Sensor" -I"../src/Sensor/Iks01a1" -I"../src/Sensor/Iks01a1/Iks01a1AccelGyro" -I"../src/Sensor/Iks01a1/Iks01a1HumidTemp" -I"../src/Sensor/Iks01a1/Iks01a1Mag" -I"../src/Sensor/Iks01a1/Iks01a1Press" -I"../src/Sensor/Iks01a1/BSP/X_NUCLEO_IKS01A1" -I"../src/Sensor/Iks01a1/BSP/Components/Common" -I"../src/Sensor/Iks01a1/BSP/Components/hts221" -I"../src/Sensor/Iks01a1/BSP/Components/lis3mdl" -I"../src/Sensor/Iks01a1/BSP/Components/lps22hb" -I"../src/Sensor/Iks01a1/BSP/Components/lps25hb" -I"../src/Sensor/Iks01a1/BSP/Components/lsm303agr" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds0" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds3" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6dsl" -I"../src/Demo" -I"../src/UserLed" -I"../src/AOWashingMachine" -I"../src/Traffic" -I"../src/Traffic/Lamp" -I"../src/Template/CompositeAct" -I"../src/Template/CompositeAct/CompositeReg" -I"../src/Template/SimpleAct" -I"../src/Template/SimpleReg" -I"../src/UartAct" -I"../src/UartAct/UartIn" -I"../src/UartAct/UartOut" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


