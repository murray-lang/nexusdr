cmake_minimum_required(VERSION 3.15)

project(nexusdr_stm32h745i_disco_cm7 C ASM)

set(CMAKE_SYSTEM_PROCESSOR cortex-m7)

set(TARGET_CM7 nexusdr_stm32h745i_disco_cm7)
set(TARGET_CM7 ${TARGET_CM7} PARENT_SCOPE)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPILE_DEFS
        STM32H745xx
        USE_STM32H745I_DISCO
        USE_HAL_DRIVER
        USE_PWR_DIRECT_SMPS_SUPPLY
        CORE_CM7
        TS_MULTI_TOUCH_SUPPORTED
        LV_DRV_NO_CONF
        LV_CONF_INCLUDE_SIMPLE
        LIMIT_FATFS_CLOCK_FOR_CM4   # Slow down SDMMC clock for CM4 FatFs access
        CFG_TUSB_MCU=OPT_MCU_STM32H7
        HSE_VALUE=25000000
)

set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")

#add_definitions(-D__cplusplus)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
    add_definitions(-DDEBUG)
    set(OPTIMISATION_FLAGS "-O0")
    set(COMPILE_DEFS ${COMPILE_DEFS} DEBUG)
else()
    set(OPTIMISATION_FLAGS "-Os")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m7 -std=gnu11 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard --specs=nano.specs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -ffunction-sections -fdata-sections -Wall -fstack-usage ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcyclomatic-complexity -MMD -MP ${OPTIMISATION_FLAGS}")

set(SOURCE_DIR_CM7 ${PLATFORM_APP_DIR}/CM7)
set(CM7_DRIVERS_DIR "${SOURCE_DIR_CM7}/Drivers")
set(CM7_APP_DIR "${SOURCE_DIR_CM7}/App")
set(CM7_SYS_DIR "${SOURCE_DIR_CM7}/Sys")
set(CM7_FREERTOS_DIR "${FREERTOS_DIR}/portable/GCC/ARM_CM7/r0p1")

set(STARTUP_FILE "${CM7_APP_DIR}/Startup/startup_stm32h745xihx.s")
#set(LD_FILE ${SOURCE_DIR_CM7}/STM32H745XIHX_FLASH.ld)
set(LD_FILE ${SOURCE_DIR_CM7}/STM32H745XIHX_QSPI.ld)

file(GLOB_RECURSE CM7_APP_SOURCES ${CM7_APP_DIR}/Src/*.c ${CM7_APP_DIR}/Src/*.cpp)

file(GLOB_RECURSE SYS_SOURCES ${SOURCE_DIR_CM7}/Sys/Src/*.c)

set(FREERTOS_SOURCES
    ${FREERTOS_COMMON_SOURCES}
    ${CMSIS_RTOS_SOURCES}
    ${CM7_FREERTOS_DIR}/port.c
    ${FREERTOS_MEMMANG_DIR}/heap_4.c
)

set(CM7_SOURCES
    ${COMMON_SOURCES}
#        ${COMMON_DIR}/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c
    ${CM7_APP_SOURCES}
    ${HAL_SOURCES}
    ${BSP_SOURCES}
    ${SYS_SOURCES}
    ${FATFS_SOURCES}
    ${LVGL_SOURCES}
    ${LVGL_DEMO_SOURCES}
    ${SOURCE_DIR_CM7}/lv_conf.h
    ${TINYUSB_SOURCES}
    ${CONFIG_SOURCES}
)

if(USE_FREERTOS)
    set(CM7_SOURCES
        ${CM7_SOURCES}
        ${FREERTOS_SOURCES}
        ${CMSIS_RTOS_SOURCES}
#        ${CMSIS_RTOS2_SOURCES}
    )
endif()

#set(CM7_CPP_SOURCES
#    ${CM7_APP_SOURCES}
#    ${HAL_SOURCES}
#)

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${DEBUG_FLAGS} -x assembler-with-cpp --specs=nano.specs")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -MMD -MP")

add_compile_options(-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)

add_link_options(-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)

add_library(CM7_STARTUP_OBJ OBJECT ${STARTUP_FILE})

add_executable(${TARGET_CM7}
        ${CM7_SOURCES}
        $<TARGET_OBJECTS:CM7_STARTUP_OBJ>
)

target_compile_definitions(${TARGET_CM7} PUBLIC ${COMPILE_DEFS})

set(INCLUDE_DIRS
        ${SOURCE_DIR_CM7}
        ${COMMON_DIR}/Inc
        ${CM7_APP_DIR}/Inc
        ${CM7_SYS_DIR}/Inc
        ${BSP_DIR}/Components/Common
        ${BSP_PLATFORM_DIR}
        ${CMSIS_DIR}/Include
        ${CMSIS_DIR}/Device/ST/STM32H7xx/Include
        ${HAL_DIR}/Inc
        ${LVGL_DIR}
        ${LVGL_DIR}/lvgl/src
        ${CMAKE_SOURCE_DIR}/include
        ${FATFS_DIR}/src
        ${TINYUSB_DIR}
        ${TINYUSB_DIR}/hw/bsp/stm32h7
        ${ARDUINO_JSON_DIR}
        ${ETL_DIR}/include
        ${CONFIG_SETTINGS_DIR}
)

message(STATUS "[CM7] ETL directory: ${ETL_DIR}")

if(USE_FREERTOS)
    set(INCLUDE_DIRS
        ${INCLUDE_DIRS}
        ${FREERTOS_DIR}/include
        ${CM7_FREERTOS_DIR}
        ${CMSIS_DIR}/Include
        ${CMSIS_RTOS_DIR}
        ${CMSIS_RTOS2_DIR}/Include
    )
endif()

target_include_directories(${TARGET_CM7} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${TARGET_CM7}
        -T${LD_FILE}
        -Wl,-Map=${TARGET_CM7}.map
        --specs=nosys.specs
        -Wl,--gc-sections
        -Wl,--start-group -lc -lm -Wl,--end-group
        stdc++
)
