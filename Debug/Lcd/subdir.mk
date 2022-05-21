################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lcd/GUI.c \
../Lcd/lcd.c \
../Lcd/lcd2.c \
../Lcd/test.c 

OBJS += \
./Lcd/GUI.o \
./Lcd/lcd.o \
./Lcd/lcd2.o \
./Lcd/test.o 

C_DEPS += \
./Lcd/GUI.d \
./Lcd/lcd.d \
./Lcd/lcd2.d \
./Lcd/test.d 


# Each subdirectory must supply rules for building sources it contributes
Lcd/%.o: ../Lcd/%.c Lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Lcd -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lcd

clean-Lcd:
	-$(RM) ./Lcd/GUI.d ./Lcd/GUI.o ./Lcd/lcd.d ./Lcd/lcd.o ./Lcd/lcd2.d ./Lcd/lcd2.o ./Lcd/test.d ./Lcd/test.o

.PHONY: clean-Lcd

