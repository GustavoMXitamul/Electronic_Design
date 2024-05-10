################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/timer_demo1.c 

OBJS += \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/timer_demo1.o 

C_DEPS += \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/timer_demo1.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32L0 -DSTM32L053R8Tx -c -I../Inc -I"C:/Users/gadol/Desktop/Codigo_arm_1/Codigo_arm_1/drivers/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/timer_demo1.cyclo ./Src/timer_demo1.d ./Src/timer_demo1.o ./Src/timer_demo1.su

.PHONY: clean-Src

