################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_ACC_driver.c \
../src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_ACC_driver_HL.c \
../src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_MAG_driver.c \
../src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_MAG_driver_HL.c 

OBJS += \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_ACC_driver.o \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_ACC_driver_HL.o \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_MAG_driver.o \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_MAG_driver_HL.o 

C_DEPS += \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_ACC_driver.d \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_ACC_driver_HL.d \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_MAG_driver.d \
./src/Sensor/Iks01a1/BSP/Components/lsm303agr/LSM303AGR_MAG_driver_HL.d 


# Each subdirectory must supply rules for building sources it contributes
src/Sensor/Iks01a1/BSP/Components/lsm303agr/%.o: ../src/Sensor/Iks01a1/BSP/Components/lsm303agr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -DUSE_STM32F4XX_NUCLEO -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/BSP/STM32F4xx-Nucleo" -I"../system/BSP/Components" -I../qpcpp/ports/arm-cm/qxk/gnu -I"../qpcpp/include" -I"../qpcpp/src" -I../STemWinLibrary522/inc -I../STemWinLibrary522/Config -I"../framework/include" -I"../src/Sensor/Iks01a1/BSP/X_NUCLEO_IKS01A1" -I"../src/Sensor/Iks01a1/BSP/Components/Common" -I"../src/Sensor/Iks01a1/BSP/Components/hts221" -I"../src/Sensor/Iks01a1/BSP/Components/lis3mdl" -I"../src/Sensor/Iks01a1/BSP/Components/lps25hb" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds0" -I"../src/Sensor/Iks01a1/BSP/Components/lps22hb" -I"../src/Sensor/Iks01a1/BSP/Components/lsm303agr" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6ds3" -I"../src/Sensor/Iks01a1/BSP/Components/lsm6dsl" -std=gnu11 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


