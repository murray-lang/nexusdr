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