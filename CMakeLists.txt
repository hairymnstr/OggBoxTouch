cmake_minimum_required(VERSION 3.24)

set(PROJ_NAME "OggBoxTouch")
project(${PROJ_NAME})

set(CMAKE_C_STANDARD 11)
enable_language(C CXX ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(UTIL_SEARCH_CMD which)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

execute_process(
    COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
    OUTPUT_VARIABLE BINUTILS_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CORE_FLAGS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

# lifted from the ogg CMake file to create the types header for our toolchain
include(CheckIncludeFiles)

# Configure config_type.h
check_include_files(inttypes.h INCLUDE_INTTYPES_H)
check_include_files(stdint.h INCLUDE_STDINT_H)
check_include_files(sys/types.h INCLUDE_SYS_TYPES_H)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/ogg/cmake")
set(SIZE16 int16_t)
set(USIZE16 uint16_t)
set(SIZE32 int32_t)
set(USIZE32 uint32_t)
set(SIZE64 int64_t)
set(USIZE64 uint64_t)

include(CheckSizes)

configure_file(Middlewares/Third_Party/ogg/include/ogg/config_types.h.in include/ogg/config_types.h @ONLY)


file(GLOB_RECURSE CORE_FILES ${CMAKE_SOURCE_DIR}/Core/Src/**.c)
file(GLOB_RECURSE DRIVER_FILES ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/**.c)
list(FILTER DRIVER_FILES EXCLUDE REGEX ".*_template\.c")
file(GLOB_RECURSE LVGL_FILES ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/lvgl/src/**.c)
file(GLOB_RECURSE TREMOR_FILES ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/tremor/*.c)
list(FILTER TREMOR_FILES EXCLUDE REGEX ".*_example\.c")

add_executable(${PROJ_NAME}
    ${CORE_FILES}
    ${DRIVER_FILES}
    ${LVGL_FILES}
    ${TREMOR_FILES}
    ${CMAKE_SOURCE_DIR}/startup_stm32f429xx.s
    ${CMAKE_SOURCE_DIR}/Application/Src/lv_port_disp.c
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Src/usbh_cdc.c
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/list.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_SOURCE_DIR}/USB_HOST/App/usb_host.c
    ${CMAKE_SOURCE_DIR}/USB_HOST/Target/usbh_conf.c
    ${CMAKE_SOURCE_DIR}/USB_HOST/Target/usbh_platform.c
)

target_compile_definitions(${PROJ_NAME} PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F429xx)

target_include_directories(${PROJ_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Host_Library/Core/Inc
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FatFs/src
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/lvgl/src
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/tremor
    ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/ogg/include
    ${CMAKE_SOURCE_DIR}/Application/Inc
    ${CMAKE_SOURCE_DIR}/USB_HOST/App
    ${CMAKE_SOURCE_DIR}/USB_HOST/Target
    ${CMAKE_SOURCE_DIR}/build/include
)

target_compile_options(${PROJ_NAME} PRIVATE
    ${CORE_FLAGS}
    -fdata-sections
    -ffunction-sections
    -Wall
    -Wextra
)

target_link_libraries(${PROJ_NAME} PRIVATE c m nosys)
target_link_options(${PROJ_NAME} PRIVATE
    ${CORE_FLAGS}
    -specs=nano.specs
    -T${CMAKE_SOURCE_DIR}/STM32F429ZITx_FLASH.ld
    -Wl,-Map=${PROJ_NAME}.map,--cref
    -Wl,--gc-sections
)

add_custom_command(TARGET ${PROJ_NAME}
    POST_BUILD
    COMMAND ln -sf ${PROJ_NAME} "${PROJ_NAME}.elf"
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJ_NAME} ${PROJ_NAME}.hex ${CUSTOM_MAKE_HEX_FLAGS}
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJ_NAME} ${PROJ_NAME}.bin ${CUSTOM_MAKE_BIN_FLAGS}
)
