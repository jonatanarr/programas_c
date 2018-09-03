################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/src/LCD.c \
../board/src/LED_S401.c \
../board/src/board.c \
../board/src/mma8451.c 

OBJS += \
./board/src/LCD.o \
./board/src/LED_S401.o \
./board/src/board.o \
./board/src/mma8451.o 

C_DEPS += \
./board/src/LCD.d \
./board/src/LED_S401.d \
./board/src/board.d \
./board/src/mma8451.d 


# Each subdirectory must supply rules for building sources it contributes
board/src/%.o: ../board/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MKL46Z256VLH4" -I"../Sources" -I../board/inc -I"../Project_Settings/Startup_Code" -I"../SDK/platform/CMSIS/Include" -I"../SDK/platform/devices" -I"../SDK/platform/devices/MKL46Z4/include" -I"C:\Freescale\KSDK_1.3.0/platform/hal/inc" -I"C:\Freescale\KSDK_1.3.0/platform/system/src/clock/MKL46Z4" -I"C:\Freescale\KSDK_1.3.0/platform/hal/src/sim/MKL46Z4" -I"C:\Freescale\KSDK_1.3.0/platform/system/inc" -I"C:\Freescale\KSDK_1.3.0/platform/osa/inc" -I../Includes -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


