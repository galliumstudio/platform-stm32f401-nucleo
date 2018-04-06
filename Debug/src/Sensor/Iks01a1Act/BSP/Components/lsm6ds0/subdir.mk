################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/LSM6DS0_ACC_GYRO_driver.c \
../src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/LSM6DS0_ACC_GYRO_driver_HL.c 

OBJS += \
./src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/LSM6DS0_ACC_GYRO_driver.o \
./src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/LSM6DS0_ACC_GYRO_driver_HL.o 

C_DEPS += \
./src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/LSM6DS0_ACC_GYRO_driver.d \
./src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/LSM6DS0_ACC_GYRO_driver_HL.d 


# Each subdirectory must supply rules for building sources it contributes
src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/%.o: ../src/Sensor/Iks01a1Act/BSP/Components/lsm6ds0/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -DUSE_STM32F4XX_NUCLEO -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/BSP/STM32F4xx-Nucleo" -I"../system/BSP/Components" -I"../src/Iks01a1/BSP/X_NUCLEO_IKS01A1" -I"../src/Iks01a1/BSP/Components/Common" -I"../src/Iks01a1/BSP/Components/hts221" -I"../src/Iks01a1/BSP/Components/lis3mdl" -I"../src/Iks01a1/BSP/Components/lps25hb" -I"../src/Iks01a1/BSP/Components/lsm6ds0" -I"../src/Iks01a1/BSP/Components/lps22hb" -I"../src/Iks01a1/BSP/Components/lsm303agr" -I"../src/Iks01a1/BSP/Components/lsm6ds3" -I"../src/Iks01a1/BSP/Components/lsm6dsl" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


