cmake_minimum_required(VERSION 3.15)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(nexusdr_stm32h745i_disco C ASM)

set(IS_STM32 ON)
add_definitions(-DIS_STM32)

# These are required to prevent build platform-specific libraries from being automagically added
# to the ld command line.
set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_CXX_STANDARD_LIBRARIES "")

set(PLATFORM_APP_DIR ${APP_DIR}/stm32h745i)

set(COMMON_DIR "${PLATFORM_APP_DIR}/Common")
set(DRIVERS_DIR "${PLATFORM_APP_DIR}/Drivers")
set(BSP_DIR "${DRIVERS_DIR}/BSP")
set(BSP_PLATFORM_DIR "${BSP_DIR}/STM32H745I-DISCO")
set(CMSIS_DIR "${DRIVERS_DIR}/CMSIS")
set(HAL_DIR "${DRIVERS_DIR}/STM32H7xx_HAL_Driver")
set(THIRD_PARTY_DIR "${PLATFORM_APP_DIR}/Middlewares/Third_Party")
set(FATFS_DIR "${THIRD_PARTY_DIR}/FatFs")
set(HAL_SRC_DIR "${HAL_DIR}/Src")
set(LVGL_DIR "${MIDDLEWARE_DIR}/lvgl")
set(TINYUSB_DIR "${MIDDLEWARE_DIR}/tinyusb_src/src")

file(GLOB COMMON_SOURCES ${COMMON_DIR}/Src/*.c)
file(GLOB HAL_SOURCES ${HAL_SRC_DIR}/*.c)
file(GLOB_RECURSE BSP_SOURCES ${BSP_DIR}/*.c)
file(GLOB_RECURSE LVGL_SOURCES ${LVGL_DIR}/lvgl/src/*.c)
file(GLOB_RECURSE LVGL_DEMO_SOURCES ${LVGL_DIR}/lvgl/demos/*.c)
file(GLOB TINYUSB_ROOT_SOURCES ${TINYUSB_DIR}/*.c)
file(GLOB TINYUSB_COMMON_SOURCES ${TINYUSB_DIR}/common/*.c)
file(GLOB TINYUSB_DEVICE_SOURCES ${TINYUSB_DIR}/device/*.c)
file(GLOB TINYUSB_CLASS_MSC_SOURCES ${TINYUSB_DIR}/class/msc/*.c)
file(GLOB TINYUSB_PORTABLE_SOURCES ${TINYUSB_DIR}/portable/synopsys/dwc2/*.c)

set(TINYUSB_SOURCES
        ${TINYUSB_ROOT_SOURCES}
        ${TINYUSB_COMMON_SOURCES}
        ${TINYUSB_DEVICE_SOURCES}
        ${TINYUSB_CLASS_MSC_SOURCES}
        ${TINYUSB_PORTABLE_SOURCES}
)

file(GLOB FATFS_SOURCES
        ${FATFS_DIR}/src/*.c
        ${FATFS_DIR}/src/drivers/*.c
        ${FATFS_DIR}/src/option/unicode.c
)

set(CM4_BINARY_DIR ${CMAKE_BINARY_DIR}/CM4)
set(CM7_BINARY_DIR ${CMAKE_BINARY_DIR}/CM7)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CM7 ${CM7_BINARY_DIR} )
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CM4 ${CM4_BINARY_DIR} )

set(EXTERNAL_LOADER "${PLATFORM_APP_DIR}/MT25TL01G_STM32H745I-DISCO.stldr")

add_custom_target(FLASH_CM4
        COMMAND STM32_Programmer_CLI --connect port=SWD --write ${CMAKE_BINARY_DIR}/CM4/${TARGET_CM4}.elf --start
        DEPENDS ${TARGET_CM4}
)

add_custom_target(FLASH_CM7
        COMMAND STM32_Programmer_CLI --connect port=SWD -el "${EXTERNAL_LOADER}" --erase all --write ${CMAKE_BINARY_DIR}/CM7/${TARGET_CM7}.elf --verify --start
        DEPENDS ${TARGET_CM7}
)