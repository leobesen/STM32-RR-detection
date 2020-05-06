################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../STM32_WPAN/App/app_ble.c \
../STM32_WPAN/App/p2p_server_app.c 

OBJS += \
./STM32_WPAN/App/app_ble.o \
./STM32_WPAN/App/p2p_server_app.o 

C_DEPS += \
./STM32_WPAN/App/app_ble.d \
./STM32_WPAN/App/p2p_server_app.d 


# Each subdirectory must supply rules for building sources it contributes
STM32_WPAN/App/app_ble.o: ../STM32_WPAN/App/app_ble.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32WB55xx -c -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/utilities -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Utilities/lpm/tiny_lpm -I../tiny_lpm -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Device/ST/STM32WBxx/Include -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/svc/Src -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Include -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../STM32_WPAN/App -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/core/template -I../Core/Inc -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/core -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Utilities/sequencer -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/core/auto -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"STM32_WPAN/App/app_ble.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32_WPAN/App/p2p_server_app.o: ../STM32_WPAN/App/p2p_server_app.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32WB55xx -c -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/utilities -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Utilities/lpm/tiny_lpm -I../tiny_lpm -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Device/ST/STM32WBxx/Include -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/svc/Src -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Include -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/svc/Inc -I../STM32_WPAN/App -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/tl -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/core/template -I../Core/Inc -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/core -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Utilities/sequencer -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/ble/core/auto -I/home/leobesen/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/ST/STM32_WPAN/interface/patterns/ble_thread/shci -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"STM32_WPAN/App/p2p_server_app.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

