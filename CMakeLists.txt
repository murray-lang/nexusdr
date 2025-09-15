cmake_minimum_required(VERSION 3.8)

project(cutesdr-vk6hl VERSION 0.1 LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --enable-threads")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --enable-threads")

#list(APPEND CMAKE_PREFIX_PATH "/home/murray/Qt/6.9.1/gcc_64/lib/cmake")
#list(APPEND CMAKE_PREFIX_PATH "/home/murray/Qt6.5/6.5.3/gcc_64/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "/home/murray/Qt/6.5.0/gcc_64/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/aarch-linux-gnu/cmake")

set(VOLK_MIN_VERSION "2.4.1")         ## first version with CPU features

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
#list(APPEND CMAKE_MODULE_PATH "/home/murray/dev/tools/rtaudio/_build_")

find_package(PkgConfig)
find_package(RtAudio REQUIRED)
#find_package(gpiod 2 QUIET CONFIG)

pkg_check_modules(GPIOD QUIET IMPORTED_TARGET libgpiod)

if(GPIOD_FOUND)
#    set(IS_RPI TRUE)
    message(STATUS "libgpiod found")
endif()

include(CMakeLists_AudioLibs.txt)
message(STATUS "${AUDIO_LINK_LIBS}")

find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Charts)
find_library(HIDAPI_LIBRARY hidapi-libusb)

message(STATUS "")
message(STATUS "Configuring VOLK support...")
find_package(Volk)
if(Volk_FOUND)
  message(STATUS "  Found VOLK:")
  message(STATUS "  * Version: ${Volk_VERSION}")
  message(STATUS "  * Libraries: ${VOLK_LIBRARIES}")
  message(STATUS "  * Includes: ${VOLK_INCLUDE_DIRS}")
  if("${Volk_VERSION}" STREQUAL "")
    message(WARNING "Empty VOLK version string. Assuming compatibility. Good luck!")
  else()
    if("${Volk_VERSION}" VERSION_LESS ${VOLK_MIN_VERSION})
      message(FATAL_ERROR "VOLK version ${Volk_VERSION} < ${VOLK_MIN_VERSION}")
    endif()
  endif()
else()
  message(WARNING "VOLK not found.")
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(USB)
    if(NOT LIBUSB_FOUND)
        message(FATAL_ERROR "libusb required to compile cutesdr-vk6hl")
    endif()
    if(HIDAPI_LIBRARY)
        MESSAGE(STATUS "System Hidapi Lib ${HIDAPI_LIBRARY} is used")
    else()
        MESSAGE(FATAL_ERROR "Hidapi required to compile cutesdr-vk6hl")
    endif()
endif()

# if(GPIOD_FOUND)
#     set (PLATFORM_SOURCES
#             src/settings/control/device/platform/rpi/GpioRotaryEncoder.h
#             src/settings/control/device/platform/rpi/GpioRotaryEncoder.cpp
#     )
# else()
#     set (PLATFORM_SOURCES
#             src/settings/control/device/platform/dummy/GpioRotaryEncoder.h
#             src/settings/control/device/platform/dummy/GpioRotaryEncoder.cpp
#     )
# endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.h mainwindow.cpp
        mainwindow.ui
        SampleTypes.h

        dsp/utils/FilterCoefficients.h
        dsp/utils/FftData.h
        dsp/blocks/Oscillator.h dsp/blocks/Oscillator.cpp
        dsp/utils/constants.h
        dsp/utils/PingPongBuffers.h
        dsp/utils/VectorAssembler.h
        dsp/utils/pocketfft/pocketfft_hdronly.h
        radio/receiver/IqReceiver.h radio/receiver/IqReceiver.cpp
        dsp/stages/IqStage.h
        dsp/stages/oscillators/OscillatorStage.h
        dsp/stages/oscillators/OscillatorMixer.h
        dsp/stages/oscillators/OscillatorInjector.h
        dsp/stages/filters/FastFIR.h
        dsp/stages/DcShift.h
        dsp/stages/demodulators/Demodulator.h
        dsp/stages/demodulators/AmDemodulator.h

        settings/control/device/usb/UsbControl.h
        settings/control/device/usb/UsbException.cpp
        settings/control/device/usb/UsbException.h
        settings/control/device/usb/LibUsbControl.cpp
        settings/control/device/usb/LibUsbControl.h
        settings/control/device/usb/HidUsbControl.cpp
        settings/control/device/usb/HidUsbControl.h
        settings/control/device/usb/UsbControl.cpp
        settings/control/ControlBase.h
        settings/control/device/FunCubeDongle/FunCubeDongle.cpp
        settings/control/device/FunCubeDongle/FunCubeDongle.h
        settings/control/ControlException.h
        settings/RfSettings.h
        settings/IfSettings.h
        settings/ReceiverSettings.h
        settings/RadioSettings.h
        settings/TransmitterSettings.h

        dsp/stages/demodulators/DemodulatorSettings.h
        dsp/stages/demodulators/AmDemodulator.cpp
        dsp/stages/filters/kernels/FirKernel.h
        dsp/stages/filters/kernels/BandPassFirKernel.h
        dsp/stages/filters/kernels/BandPassFirKernel.cpp
        dsp/stages/filters/kernels/BandStopFirKernel.cpp
        dsp/stages/filters/kernels/BandStopFirKernel.h
        dsp/stages/filters/kernels/LowPassFirKernel.cpp
        dsp/stages/filters/kernels/LowPassFirKernel.h
        SignalEmitter.h
        dsp/stages/filters/OverlapBuffers.h
        dsp/utils/window.h
        dsp/utils/OverlapBuffers.h
        dsp/stages/decimator/Decimator.h
        dsp/stages/decimator/MyFirCoefficients.h

        dsp/stages/metering/MeteringStage.h
        config/RadioConfig.h
        config/ReceiverConfig.h
        config/TransmitterConfig.h
        config/AudioConfig.h
        config/ConfigBase.h
        config/ConfigException.h

        io/audio/device/AudioDevice.h
        io/audio/IqAudioInput.h
        io/audio/device/AudioOutputDevice.cpp
        io/audio/device/AudioOutputDevice.h
        io/audio/device/IqAudioInputDevice.h
        io/audio/AudioException.h
        io/audio/AudioException.cpp
        io/audio/IqSink.h
        io/audio/AudioIo.h
        io/audio/AudioOutput.h
        radio/receiver/ReceiverIqEvent.h
        radio/receiver/ReceiverIqEvent.cpp
        radio/receiver/ReceiverAudioEvent.cpp
        radio/receiver/ReceiverAudioEvent.h
        settings/control/ControlSinkFactory.h
        settings/control/ControlSinkFactory.cpp
        io/audio/device/AudioDeviceFactory.h
        settings/ReceiverSettingsSink.h
        settings/PttSink.h
        radio/Radio.cpp
        radio/Radio.h
        settings/RadioSettingsSink.h
        config/ControlConfig.h
        settings/control/ControlSink.cpp
        settings/control/ControlSink.h
        settings/RadioSettingsSource.h
        settings/control/ControlSource.cpp
        settings/control/ControlSource.h
        settings/PttSource.h
        settings/control/ControlSourceFactory.cpp
        settings/control/ControlSourceFactory.h
        ${PLATFORM_SOURCES}
        settings/control/device/gpio/Gpio.cpp
        settings/control/device/gpio/Gpio.h
        settings/control/device/gpio/GpioException.cpp
        settings/control/device/gpio/GpioException.h
        settings/control/device/gpio/GpioLines.cpp
        settings/control/device/gpio/GpioLines.h
        settings/control/device/gpio/digital/GpioRotaryEncoder.cpp
        settings/control/device/gpio/digital/GpioRotaryEncoder.h
        settings/control/device/gpio/digital/DigitalInputGroup.cpp
        settings/control/device/gpio/digital/DigitalInputGroup.h
        settings/SettingPath.h
        settings/SettingDelta.h
        settings/SettingsException.h
        settings/SettingsBase.h
        util/StringUtils.cpp
        util/StringUtils.h
        settings/control/RadioControl.cpp
        settings/control/RadioControl.h
        dsp/stages/demodulators/FmDemodulator.cpp
        dsp/stages/demodulators/FmDemodulator.h
        settings/control/device/gpio/digital/DigitalInput.h
        settings/control/device/gpio/digital/DigitalInput.cpp
        settings/control/device/gpio/digital/DigitalInputFactory.cpp
        settings/control/device/gpio/digital/DigitalInputFactory.h
        settings/control/device/gpio/digital/config/DigitalInputConfig.cpp
        settings/control/device/gpio/digital/config/DigitalInputConfig.h
        settings/control/device/gpio/digital/DigitalInputFactory.cpp
        config/VariantConfig.h
        config/ConfigFactory.cpp
        config/ConfigFactory.h
        config/DigitalInputGroupConfig.h
        config/RotaryEncoderConfig.h
        config/DigitalInputConfig.h
        config/FunCubeConfig.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(cutesdr-vk6hl
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET cutesdr-vk6hl APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(cutesdr-vk6hl SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(cutesdr-vk6hl
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(cutesdr-vk6hl PUBLIC
    ${CMAKE_SOURCE_DIR}
    /usl/local/include
    /usr/local/include/rtaudio
    ${VOLK_INCLUDE_DIRS}
    /usr/include/hidapi/
)
target_link_directories(cutesdr-vk6hl PRIVATE ${DSPL_DIR})

set (COMMON_LIBRARIES
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    /usr/local/lib/librtaudio.so
    ${AUDIO_LINK_LIBS}
    ${VOLK_LIBRARIES}
    usb-1.0
    hidapi-libusb
    nlohmann_json::nlohmann_json
)

if(GPIOD_FOUND)
   set (PLATFORM_LIBRARIES
     PkgConfig::GPIOD
   )
else()
endif()

target_link_libraries(cutesdr-vk6hl
    PRIVATE
    ${COMMON_LIBRARIES}
    ${PLATFORM_LIBRARIES}
)

set_target_properties(cutesdr-vk6hl PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS cutesdr-vk6hl
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cutesdr-vk6hl)
endif()
