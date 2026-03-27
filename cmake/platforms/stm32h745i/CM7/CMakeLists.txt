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
)

set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")

#add_definitions(-D__cplusplus)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
    add_definitions(-DDEBUG)
    set(OPTIMISATION_FLAGS "-O0")
else()
    set(OPTIMISATION_FLAGS "-Os")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m7 -std=gnu11 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard --specs=nano.specs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -ffunction-sections -fdata-sections -Wall -fstack-usage ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcyclomatic-complexity -MMD -MP ${OPTIMISATION_FLAGS}")

set(SOURCE_DIR_CM7 ${PLATFORM_APP_DIR}/CM7)
set(LVGL_DIR "${APP_DIR}/lvgl")

set(COMMON_DIR "${PLATFORM_APP_DIR}/Common")
set(COMMON_DRIVERS_DIR "${PLATFORM_APP_DIR}/Drivers")
set(THIRD_PARTY_DIR "${PLATFORM_APP_DIR}/Middlewares/Third_Party")
set(FATFS_DIR "${THIRD_PARTY_DIR}/FatFs")
set(CM7_DRIVERS_DIR "${SOURCE_DIR_CM7}/Drivers")
set(CM7_CORE_DIR "${SOURCE_DIR_CM7}/Core")

set(BSP_DIR "${CM7_DRIVERS_DIR}/BSP")
set(BSP_PLATFORM_DIR "${BSP_DIR}/STM32H745I-DISCO")
set(BSP_COMPONENTS_DIR "${BSP_DIR}/Components")
set(CMSIS_DIR "${COMMON_DRIVERS_DIR}/CMSIS")
set(HAL_DIR "${COMMON_DRIVERS_DIR}/STM32H7xx_HAL_Driver")
set(HAL_SRC_DIR "${HAL_DIR}/Src")

set(APP_DIR "${SOURCE_DIR_CM7}/Application")


set(STARTUP_FILE "${CM7_CORE_DIR}/Startup/startup_stm32h745xihx.s")
#set(LD_FILE ${SOURCE_DIR_CM7}/STM32H745XIHX_FLASH.ld)
set(LD_FILE ${SOURCE_DIR_CM7}/STM32H745XIHX_QSPI.ld)

file(GLOB_RECURSE CM7_CORE_SOURCES ${CM7_CORE_DIR}/Src/*.c ${CM7_CORE_DIR}/Src/*.cpp)
file(GLOB COMMON_SOURCES ${COMMON_DIR}/Src/*.c)
file(GLOB HAL_SOURCES ${HAL_SRC_DIR}/*.c)
file(GLOB_RECURSE BSP_SOURCES ${BSP_DIR}/*.c)
file(GLOB_RECURSE LVGL_SOURCES ${LVGL_DIR}/lvgl/src/*.c)
file(GLOB_RECURSE LVGL_DEMO_SOURCES ${LVGL_DIR}/lvgl/demos/*.c)
file(GLOB SYS_SOURCES ${SOURCE_DIR_CM7}/Sys/*.c)
file(GLOB FATFS_SOURCES ${FATFS_DIR}/src/*.c)

set(CM7_SOURCES
    ${COMMON_SOURCES}
#        ${COMMON_DIR}/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c
    ${CM7_CORE_SOURCES}
    ${HAL_SOURCES}
    ${BSP_SOURCES}
    ${SYS_SOURCES}
    ${FATFS_SOURCES}
    ${LVGL_SOURCES}
    ${LVGL_DEMO_SOURCES}
    ${SOURCE_DIR_CM7}/lv_conf.h
)

set(CM7_CPP_SOURCES
#        ${COMMON_SOURCES}
        ${CM7_CORE_SOURCES}
        ${HAL_SOURCES}
#        ${BSP_SOURCES}
#        ${FATFS_SOURCES}
)

#set_source_files_properties(${CM7_CPP_SOURCES} PROPERTIES LANGUAGE CXX)
#set_source_files_properties(${BSP_SOURCES} ${FATFS_SOURCES} PROPERTIES LANGUAGE C)

#set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${DEBUG_FLAGS} -mcpu=cortex-m7 -x assembler-with-cpp --specs=nano.specs")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${DEBUG_FLAGS} -x assembler-with-cpp --specs=nano.specs")
#set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -MMD -MP")
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
        ${CM7_CORE_DIR}/Inc
        ${BSP_DIR}/Components/Common
        ${BSP_PLATFORM_DIR}
        ${CMSIS_DIR}/Include
        ${CMSIS_DIR}/Device/ST/STM32H7xx/Include
        ${HAL_DIR}/Inc
        ${LVGL_DIR}
        ${LVGL_DIR}/lvgl/src
        ${CMAKE_SOURCE_DIR}/include
        ${FATFS_DIR}/src
)

target_include_directories(${TARGET_CM7} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${TARGET_CM7}
        -T${LD_FILE}
        -Wl,-Map=${TARGET_CM7}.map
        --specs=nosys.specs
        -Wl,--gc-sections
        -Wl,--start-group -lc -lm -Wl,--end-group
        stdc++
)
