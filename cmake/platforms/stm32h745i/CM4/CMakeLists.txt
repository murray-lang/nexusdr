cmake_minimum_required(VERSION 3.15)

project(nexusdr_stm32h745i_disco_cm4 C ASM)

set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

set(TARGET_CM4 nexusdr_stm32h745i_disco_cm4)
set(TARGET_CM4 ${TARGET_CM4} PARENT_SCOPE)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPILE_DEFS
        STM32H745xx
        USE_STM32H745I_DISCO
        USE_HAL_DRIVER
        USE_PWR_DIRECT_SMPS_SUPPLY
        CORE_CM4
        DISABLE_MMC_INIT # CM7 is responsible for MMC init.
        LIMIT_FATFS_CLOCK_FOR_CM4   # Slow down SDMMC clock for CM4 FatFs access
        CFG_TUSB_MCU=OPT_MCU_STM32H7
        HSE_VALUE=25000000
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4 -std=gnu11 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard --specs=nano.specs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -ffunction-sections -fdata-sections -Wall -fstack-usage ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcyclomatic-complexity -MMD -MP")

set(SOURCE_DIR_CM4 ${PLATFORM_APP_DIR}/CM4)

set(CM4_APP_DIR "${SOURCE_DIR_CM4}/App")
set(CM4_SYS_DIR "${SOURCE_DIR_CM4}/Sys")
set(CM4_FREERTOS_DIR "${FREERTOS_DIR}/portable/GCC/ARM_CM4F")

set(STARTUP_FILE "${CM4_APP_DIR}/Src/Startup/startup_stm32h745xihx.s")
set(LD_FILE ${SOURCE_DIR_CM4}/STM32H745XIHX_FLASH.ld)

file(GLOB_RECURSE CM4_APP_SOURCES ${CM4_APP_DIR}/Src/*.c)
file(GLOB_RECURSE CM4_SYS_SOURCES ${CM4_SYS_DIR}/Src/*.c)
file(GLOB HAL_SOURCES ${HAL_SRC_DIR}/*.c)

set(FREERTOS_SOURCES
    ${FREERTOS_COMMON_SOURCES}
    ${CMSIS_RTOS_SOURCES}
    ${CM4_FREERTOS_DIR}/port.c
    ${FREERTOS_MEMMANG_DIR}/heap_4.c
)

set(CM4_SOURCES
    ${COMMON_DIR}/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c
    ${COMMON_SOURCES}
    ${HAL_SOURCES}
    ${BSP_SOURCES}
    ${FATFS_SOURCES}
    ${CM4_APP_SOURCES}
    ${CM4_SYS_SOURCES}
#    ${TINYUSB_SOURCES}
)

if(USE_FREERTOS)
    set(CM4_SOURCES
        ${CM4_SOURCES}
        ${FREERTOS_SOURCES}
        ${CMSIS_RTOS_SOURCES}
    )
endif()

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${DEBUG_FLAGS} -x assembler-with-cpp --specs=nano.specs")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -MMD -MP")

add_compile_options(-mcpu=cortex-m4 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)

add_link_options(-mcpu=cortex-m4 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)

add_library(CM4_STARTUP_OBJ OBJECT ${STARTUP_FILE})

add_executable(${TARGET_CM4}
        ${CM4_SOURCES}
        $<TARGET_OBJECTS:CM4_STARTUP_OBJ>
)

target_compile_definitions(${TARGET_CM4} PUBLIC ${COMPILE_DEFS})

set(INCLUDE_DIRS
    ${CM4_APP_DIR}/Inc
    ${CM4_SYS_DIR}/Inc
    ${COMMON_DIR}/Inc
    ${BSP_DIR}/Components/Common
    ${BSP_PLATFORM_DIR}
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/Device/ST/STM32H7xx/Include
    ${HAL_DIR}/Inc
    ${FATFS_DIR}/src
#    ${TINYUSB_DIR}
#    ${TINYUSB_DIR}/hw/bsp/stm32h7
)

if(USE_FREERTOS)
    set(INCLUDE_DIRS
        ${INCLUDE_DIRS}
        ${FREERTOS_DIR}/include
        ${CM4_FREERTOS_DIR}
        ${CMSIS_DIR}/Include
        ${CMSIS_RTOS_DIR}
        ${CMSIS_RTOS2_DIR}/Include
    )
endif()

target_include_directories(${TARGET_CM4} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${TARGET_CM4}
        -T${LD_FILE}
        -Wl,-Map=${TARGET_CM4}.map
        --specs=nosys.specs
        -Wl,--gc-sections
        -Wl,--start-group -lc -lm -Wl,--end-group
)
