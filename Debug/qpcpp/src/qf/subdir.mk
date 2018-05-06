################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../qpcpp/src/qf/qep_hsm.cpp \
../qpcpp/src/qf/qep_msm.cpp \
../qpcpp/src/qf/qf_act.cpp \
../qpcpp/src/qf/qf_actq.cpp \
../qpcpp/src/qf/qf_defer.cpp \
../qpcpp/src/qf/qf_dyn.cpp \
../qpcpp/src/qf/qf_mem.cpp \
../qpcpp/src/qf/qf_ps.cpp \
../qpcpp/src/qf/qf_qact.cpp \
../qpcpp/src/qf/qf_qeq.cpp \
../qpcpp/src/qf/qf_qmact.cpp \
../qpcpp/src/qf/qf_time.cpp 

OBJS += \
./qpcpp/src/qf/qep_hsm.o \
./qpcpp/src/qf/qep_msm.o \
./qpcpp/src/qf/qf_act.o \
./qpcpp/src/qf/qf_actq.o \
./qpcpp/src/qf/qf_defer.o \
./qpcpp/src/qf/qf_dyn.o \
./qpcpp/src/qf/qf_mem.o \
./qpcpp/src/qf/qf_ps.o \
./qpcpp/src/qf/qf_qact.o \
./qpcpp/src/qf/qf_qeq.o \
./qpcpp/src/qf/qf_qmact.o \
./qpcpp/src/qf/qf_time.o 

CPP_DEPS += \
./qpcpp/src/qf/qep_hsm.d \
./qpcpp/src/qf/qep_msm.d \
./qpcpp/src/qf/qf_act.d \
./qpcpp/src/qf/qf_actq.d \
./qpcpp/src/qf/qf_defer.d \
./qpcpp/src/qf/qf_dyn.d \
./qpcpp/src/qf/qf_mem.d \
./qpcpp/src/qf/qf_ps.d \
./qpcpp/src/qf/qf_qact.d \
./qpcpp/src/qf/qf_qeq.d \
./qpcpp/src/qf/qf_qmact.d \
./qpcpp/src/qf/qf_time.d 


# Each subdirectory must supply rules for building sources it contributes
qpcpp/src/qf/%.o: ../qpcpp/src/qf/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/BSP/STM32F4xx-Nucleo" -I"../system/BSP/Components" -I../qpcpp/ports/arm-cm/qxk/gnu -I../qpcpp/include -I../qpcpp/src -I"../framework/include" -I"../src/Console" -I"../src/Console/CmdInput" -I"../src/Console/CmdParser" -I"../src/System" -I"../src/GpioInAct" -I"../src/GpioInAct/GpioIn" -I"../src/WifiSt" -I"../src/GpioOutAct" -I"../src/GpioOutAct/GpioOut" -I"../src/Sensor" -I"../src/Sensor/Iks01a1" -I"../src/Sensor/Iks01a1/Iks01a1AccelGyro" -I"../src/Sensor/Iks01a1/Iks01a1HumidTemp" -I"../src/Sensor/Iks01a1/Iks01a1Mag" -I"../src/Sensor/Iks01a1/Iks01a1Press" -I"../src/Sensor/Iks01a1/BSP/X_NUCLEO_IKS01A1" -I"../src/Sensor/Iks01a1/BSP/Components/Common" -I"../src/Sensor/Iks01a1/BSP/Components/hts221" -I"../src/Sensor/Iks01a1/BSP/Components/lis3mdl" -I"../src/Sensor/Iks01a1/BSP/Components/lps22hb" -I"../src/Sensor/Iks01a1/BSP/Components/lps25hb" -I"../src/Sensor/Iks01a1/BSP/Components/lsm303agr" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds0" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds3" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6dsl" -I"../src/Demo" -I"../src/UserLed" -I"../src/AOWashingMachine" -I"../src/Traffic" -I"../src/Traffic/Lamp" -I"../src/Template/CompositeAct" -I"../src/Template/CompositeAct/CompositeReg" -I"../src/Template/SimpleAct" -I"../src/Template/SimpleReg" -I"../src/UartAct" -I"../src/UartAct/UartIn" -I"../src/UartAct/UartOut" -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


