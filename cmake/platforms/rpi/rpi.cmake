cmake_minimum_required(VERSION 3.15)

project(nexusdr-rpi VERSION 0.1 LANGUAGES CXX)

set(USE_GUI ON)
set(USE_GPIO ON)

set(IS_RPI ON)

add_definitions(-DUSE_GPIO)

set(IS_LINUX ON)
set(IS_QT ON)

add_definitions(-DIS_QT)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(PkgConfig)

find_path(LIQUID_INCLUDE_DIR NAMES liquid/liquid.h) # TODO modify this per TARGET_PLATFORM
find_library(LIQUID_LIBRARY NAMES liquid)
find_package(nlohmann_json 3.11.2 REQUIRED)

if(LIQUID_INCLUDE_DIR AND LIQUID_LIBRARY)
    message(STATUS "Found liquid-dsp: ${LIQUID_LIBRARY}")
else()
    message(FATAL_ERROR "liquid-dsp not found!")
endif()

find_package(RtAudio REQUIRED)

if(USE_GPIO)
    pkg_check_modules(GPIOD IMPORTED_TARGET libgpiod)
    if(GPIOD_FOUND)
        message(STATUS "libgpiod found")
        add_definitions(-DUSE_GPIOD)
        set(USE_GPIOD ON)
    else()
        message(FATAL_ERROR "USE_GPIO is set to true, but libgpiod not found")
    endif()
    set(USE_PIGPIO OFF) #PIGPIO causes problems with modern RPi distros. Keeping reference just in case
    add_definitions(-DMAX_GPIO=27)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/CMakeLists_AudioLibs.txt)
message(STATUS "${AUDIO_LINK_LIBS}")

find_library(HIDAPI_LIBRARY hidapi-libusb)
find_package(USB)
if(NOT USB_FOUND)
    message(FATAL_ERROR "libusb required to compile nexusdr")
endif()
if(HIDAPI_LIBRARY)
    MESSAGE(STATUS "System Hidapi Lib ${HIDAPI_LIBRARY} is used")
else()
    MESSAGE(FATAL_ERROR "Hidapi required to compile nexusdr")
endif()

if (USE_GUI)
    find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Charts)
    #find_package(Qwt REQUIRED)
    #if(QWT_FOUND)
    #    message("Qwt found")
    #else()
    #    message("Qwt NOT found")
    #endif()
else()
    find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
endif()

if(USE_PIGPIO)
    set (GPIO_IMPL_SOURCES
            src/io/control/device/gpio/impl/pigpio/GpioImplPiGpio.h
            src/io/control/device/gpio/impl/pigpio/GpioImplPiGpio.cpp
            src/io/control/device/gpio/impl/pigpio/DigitalInputsRequestImplPiGpio.h
            src/io/control/device/gpio/impl/pigpio/DigitalInputsRequestImplPiGpio.cpp
            src/io/control/device/gpio/impl/pigpio/DigitalOutputsRequestImplPiGpio.h
            src/io/control/device/gpio/impl/pigpio/DigitalOutputsRequestImplPiGpio.cpp
    )

elseif(USE_GPIOD)
    set (GPIO_IMPL_SOURCES
            src/io/control/device/gpio/impl/gpiod/GpioImplGpiod.h
            src/io/control/device/gpio/impl/gpiod/GpioImplGpiod.cpp
            src/io/control/device/gpio/impl/gpiod/DigitalInputLinesRequestImplGpiod.h
            src/io/control/device/gpio/impl/gpiod/DigitalInputLinesRequestImplGpiod.cpp
            src/io/control/device/gpio/impl/gpiod/DigitalOutputLinesRequestImplGpiod.cpp
            src/io/control/device/gpio/impl/gpiod/DigitalOutputLinesRequestImplGpiod.h
    )
endif()

if (RTAUDIO_FOUND)
    set (AUDIO_DRIVER_SOURCES
            src/io/audio/drivers/RtAudio/RtAudioDriver.h
            src/io/audio/drivers/RtAudio/RtAudioOutputDriver.h
            src/io/audio/drivers/RtAudio/RtAudioInputDriver.h
            src/io/audio/drivers/RtAudio/AudioDriverFactory.h
    )
    set (AUDIO_DRIVER_INCLUDE_DIRS
            /usl/local/include
            /usr/local/include/rtaudio
            src/io/audio/drivers/RtAudio
    )
    set (AUDIO_DRIVER_LIBS
            /usr/local/lib/librtaudio.so
    )
endif()

set(USB_API_SOURCES
        src/io/control/device/usb/linux/LibUsbControl.cpp
        src/io/control/device/usb/linux/HidUsbControl.cpp
)

set(GPIO_SOURCES
        ${GPIO_IMPL_SOURCES}
        src/io/control/device/gpio/Gpio.cpp
        src/io/control/device/gpio/Gpio.h
        src/io/control/device/gpio/GpioException.cpp
        src/io/control/device/gpio/GpioException.h
        src/io/control/device/gpio/digital/DigitalInputLinesRequest.cpp
        src/io/control/device/gpio/digital/DigitalInputLinesRequest.h
        src/io/control/device/gpio/digital/GpioRotaryEncoder.cpp
        src/io/control/device/gpio/digital/GpioRotaryEncoder.h
        src/io/control/device/gpio/digital/DigitalInputs.cpp
        src/io/control/device/gpio/digital/DigitalInputs.h
        src/io/control/device/gpio/digital/DigitalInput.h
        src/io/control/device/gpio/digital/DigitalInput.cpp
        src/io/control/device/gpio/digital/DigitalInputFactory.cpp
        src/io/control/device/gpio/digital/DigitalInputFactory.h
        src/io/control/device/gpio/digital/DigitalInputFactory.cpp
        src/io/control/device/gpio/GpioLines.cpp
        src/io/control/device/gpio/GpioLines.h
        src/io/control/device/gpio/digital/DigitalOutput.cpp
        src/io/control/device/gpio/digital/DigitalOutput.h
        src/io/control/device/gpio/digital/DigitalOutputLinesRequest.h
        src/io/control/device/gpio/digital/DigitalOutputLinesRequest.cpp
        src/io/control/device/gpio/digital/GpioBandSelector.cpp
        src/io/control/device/gpio/digital/GpioBandSelector.h
)

set(EVENT_SOURCES
        ${EVENT_SOURCES}
        src/core/events/qt/EventDispatcher.cpp
        src/core/config-settings/settings/events/qt/RadioSettingsEvent.cpp
        src/core/config-settings/settings/events/qt/SettingUpdateEvent.cpp
        src/core/radio/receiver/events/qt/ReceiverMeterEvent.cpp

        src/core/radio/transmitter/events/qt/TransmitterAudioEvent.cpp
        src/core/radio/receiver/events/qt/ReceiverIqEvent.cpp
        src/core/radio/receiver/events/qt/ReceiverAudioEvent.cpp
        src/core/radio/transmitter/events/qt/TransmitterIqEvent.cpp
)

set (RADIO_SOURCES
        ${RADIO_BASE_SOURCES}
        ${RADIO_MASTER_SOURCES}
        ${RADIO_ENGINE_SOURCES}
        src/core/radio/Radio.cpp
        src/core/radio/Radio.h
)

set (CORE_SOURCES
        ${CONFIG_SOURCES}
        ${SETTINGS_SOURCES}
        ${DSP_SOURCES}
        ${IO_SOURCES}
        ${RADIO_SOURCES}
        ${EVENT_SOURCES}
        ${UTIL_SOURCES}
        ${USB_API_SOURCES}
        src/core/SampleTypes.h
)

if(USE_GUI)
    set(QT_GUI_SOURCES
            src/app/qt/common/QtChartTheme.h
            src/app/qt/common/QtBandDialog.cpp
            src/app/qt/common/QtBandDialog.h
            src/app/qt/common/QtChartBase.cpp
            src/app/qt/common/QtChartBase.h
            src/app/qt/common/QtTimeSeriesChart.cpp
            src/app/qt/common/QtTimeSeriesChart.h
            src/app/qt/common/QtPanadapter.cpp
            src/app/qt/common/QtPanadapter.h
            src/app/qt/common/FrequencyPanel/QtFrequencyPanel.cpp
            src/app/qt/common/FrequencyPanel/QtFrequencyPanel.h
            src/app/qt/common/QtNumberReadout.cpp
            src/app/qt/common/QtNumberReadout.h
            src/app/qt/common/FrequencyPanel/QtBandReadout.cpp
            src/app/qt/common/FrequencyPanel/QtBandReadout.h
            src/app/qt/common/FrequencyPanel/VfoReadout/QtVfoReadout.cpp
            src/app/qt/common/FrequencyPanel/VfoReadout/QtVfoReadout.h
            src/app/qt/common/MiniVfoToolbar/QtMiniVfoToolbar.ui
            src/app/qt/common/QWidgetPropertySetter.cpp
            src/app/qt/common/QWidgetPropertySetter.h
            src/app/qt/common/FrequencyPanel/VfoReadout/VfoActions.h
            src/app/qt/common/MiniVfoToolbar/QtMiniVfoToolbar.cpp
            src/app/qt/common/MiniVfoToolbar/QtMiniVfoToolbar.h
            src/app/qt/common/ScaleRenderer/ScaleRenderer.cpp
            src/app/qt/common/ScaleRenderer/ScaleRenderer.h
            src/app/qt/common/SMeter/SMeterTickProvider.cpp
            src/app/qt/common/SMeter/SMeterTickProvider.h
            src/app/qt/common/ScaleRenderer/LinearTickProvider.cpp
            src/app/qt/common/ScaleRenderer/LinearTickProvider.h
            src/app/qt/common/ScaleRenderer/NiceNumberTickProvider.cpp
            src/app/qt/common/ScaleRenderer/NiceNumberTickProvider.h
            src/app/qt/common/ScaleRenderer/ScaleModelHelpers.h
            src/app/qt/common/ScaleRenderer/ScaleLayoutHelpers.h
            src/app/qt/common/SMeter/QtSMeter.cpp
            src/app/qt/common/SMeter/QtSMeter.h
            src/app/qt/faces/FaceBase.h
            src/app/qt/faces/FaceFactory.cpp
            src/app/qt/faces/FaceFactory.h
            src/app/qt/faces/standard/StandardFace.cpp
            src/app/qt/faces/standard/StandardFace.h
            src/app/qt/faces/standard/StandardFace.ui
            src/app/qt/faces/RegisterFace.h
    )

    set(QT_APP_SOURCES
            src/app/qt/main.cpp
            src/app/qt/mainwindow.h src/app/qt/mainwindow.cpp
            src/app/qt/mainwindow-1024x600.ui
            src/app/qt/resources.qrc
    )
endif()
set(APP_SOURCES ${QT_GUI_SOURCES} ${QT_APP_SOURCES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(nexusdr
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
    )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET nexusdr APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(nexusdr SHARED ${PROJECT_SOURCES} )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(nexusdr ${PROJECT_SOURCES} )
    endif()
endif()

if(USE_PIGPIO)
    set(GPIO_LIBRARIES
            ${PIGPIO_LIBRARY}
    )
    set(GPIO_INCLUDE_DIRS
            ${PIGPIO_INCLUDE_DIR}
    )
elseif(USE_GPIOD)
    set (GPIO_LIBRARIES
            PkgConfig::GPIOD
    )
endif()

set(INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${AUDIO_DRIVER_INCLUDE_DIRS}
        ${LIQUID_INCLUDE_DIR}
        /usr/include/hidapi/
        ${GPIO_INCLUDE_DIRS}
)

target_include_directories(nexusdr PUBLIC ${INCLUDE_DIRS} )
#target_link_directories(nexusdr PRIVATE ${DSPL_DIR})

set(PROJECT_LIBRARIES
        nlohmann_json::nlohmann_json
        ${LIQUID_LIBRARY}
        ${AUDIO_DRIVER_LIBS}
        ${AUDIO_LINK_LIBS}
        usb-1.0
        hidapi-libusb
        Qt${QT_VERSION_MAJOR}::Core
        ${GPIO_LIBRARIES}
)

if(USE_GUI)
    set(PROJECT_LIBRARIES
            ${PROJECT_LIBRARIES}
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Charts
    )
endif()

target_link_libraries(nexusdr PRIVATE ${PROJECT_LIBRARIES})

set_target_properties(nexusdr PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}"
)
install(TARGETS nexusdr
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(nexusdr)
endif()