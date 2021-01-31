################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CPP/Src/DigitalPin.cpp \
../CPP/Src/LCDController.cpp \
../CPP/Src/main.cpp 

OBJS += \
./CPP/Src/DigitalPin.o \
./CPP/Src/LCDController.o \
./CPP/Src/main.o 

CPP_DEPS += \
./CPP/Src/DigitalPin.d \
./CPP/Src/LCDController.d \
./CPP/Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
CPP/Src/DigitalPin.o: ../CPP/Src/DigitalPin.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0plus -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32L053xx -DDEBUG -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I"C:/Users/JPKSP/STM32CubeIDE/workspace_1.2.0/SteadyHandMain/CPP" -I"C:/Users/JPKSP/STM32CubeIDE/workspace_1.2.0/SteadyHandMain/CPP/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"CPP/Src/DigitalPin.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
CPP/Src/LCDController.o: ../CPP/Src/LCDController.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0plus -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32L053xx -DDEBUG -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I"C:/Users/JPKSP/STM32CubeIDE/workspace_1.2.0/SteadyHandMain/CPP" -I"C:/Users/JPKSP/STM32CubeIDE/workspace_1.2.0/SteadyHandMain/CPP/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"CPP/Src/LCDController.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
CPP/Src/main.o: ../CPP/Src/main.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0plus -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32L053xx -DDEBUG -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I"C:/Users/JPKSP/STM32CubeIDE/workspace_1.2.0/SteadyHandMain/CPP" -I"C:/Users/JPKSP/STM32CubeIDE/workspace_1.2.0/SteadyHandMain/CPP/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"CPP/Src/main.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

