cmake_minimum_required(VERSION 3.8)

project(cutesdr-vk6hl VERSION 0.1 LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --enable-threads")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --enable-threads")

# list(APPEND CMAKE_PREFIX_PATH "/home/murray/Qt/6.9.1/gcc_64/lib/cmake")
#list(APPEND CMAKE_PREFIX_PATH "/home/murray/Qt6.5/6.5.3/gcc_64/lib/cmake")
#list(APPEND CMAKE_PREFIX_PATH "/home/murray/Qt/6.5.0/gcc_64/lib/cmake")
#list(APPEND CMAKE_PREFIX_PATH "/usr/lib/aarch-linux-gnu/cmake")
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake")

set(VOLK_MIN_VERSION "2.4.1")         ## first version with CPU features

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(PkgConfig)
find_package(RtAudio REQUIRED)

#pkg_check_modules(SAMPLERATE REQUIRED samplerate)

find_path(LIQUID_INCLUDE_DIR NAMES liquid/liquid.h)
find_library(LIQUID_LIBRARY NAMES liquid)

if(LIQUID_INCLUDE_DIR AND LIQUID_LIBRARY)
    message(STATUS "Found liquid-dsp: ${LIQUID_LIBRARY}")
else()
    message(FATAL_ERROR "liquid-dsp not found!")
endif()


# find_path(PIGPIO_INCLUDE_DIR pigpio.h PATHS /usr/local/include /usr/include)   
# find_library(PIGPIO_LIBRARY NAMES pigpio PATHS /usr/local/lib /usr/lib)


# Prefer pigpio if available
# if(PIGPIO_INCLUDE_DIR AND PIGPIO_LIBRARY)
if(false)
   set(IS_RPI TRUE)
    message(STATUS "pigpio found")
    add_definitions(-DUSE_PIGPIO)
    set(USE_PIGPIO ON)
else()
    # Otherwise GPIOD will have to do.
    set(USE_PIGPIO OFF)
    pkg_check_modules(GPIOD QUIET IMPORTED_TARGET libgpiod)

    if(GPIOD_FOUND)
        message(STATUS "libgpiod found")
        add_definitions(-DUSE_GPIOD)
        set(USE_GPIOD ON)
    endif()
endif()

add_definitions(-DMAX_GPIO=27)

include(CMakeLists_AudioLibs.txt)
message(STATUS "${AUDIO_LINK_LIBS}")

find_package(nlohmann_json 3.11.2 REQUIRED)
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

if(USE_PIGPIO)
    set (PLATFORM_SOURCES
            io/control/device/gpio/impl/pigpio/GpioImplPiGpio.h
            io/control/device/gpio/impl/pigpio/GpioImplPiGpio.cpp
            io/control/device/gpio/impl/pigpio/DigitalInputsRequestImplPiGpio.h
            io/control/device/gpio/impl/pigpio/DigitalInputsRequestImplPiGpio.cpp
            io/control/device/gpio/impl/pigpio/DigitalOutputsRequestImplPiGpio.h
            io/control/device/gpio/impl/pigpio/DigitalOutputsRequestImplPiGpio.cpp
    )
    
elseif(USE_GPIOD)
    set (PLATFORM_SOURCES
            io/control/device/gpio/impl/gpiod/GpioImplGpiod.h
            io/control/device/gpio/impl/gpiod/GpioImplGpiod.cpp
            io/control/device/gpio/impl/gpiod/DigitalInputLinesRequestImplGpiod.h
            io/control/device/gpio/impl/gpiod/DigitalInputLinesRequestImplGpiod.cpp
            io/control/device/gpio/impl/gpiod/DigitalOutputLinesRequestImplGpiod.cpp
            io/control/device/gpio/impl/gpiod/DigitalOutputLinesRequestImplGpiod.h
    )
endif()

if (RTAUDIO_FOUND)
    set (AUDIO_DRIVER_SOURCES
            io/audio/drivers/RtAudio/RtAudioDriver.h
            io/audio/drivers/RtAudio/RtAudioOutputDriver.h
            io/audio/drivers/RtAudio/RtAudioInputDriver.h
            io/audio/drivers/RtAudio/AudioDriverFactory.h
    )
    set (AUDIO_DRIVER_INCLUDE_DIRS
            /usl/local/include
            /usr/local/include/rtaudio
            io/audio/drivers/RtAudio
    )
    set (AUDIO_DRIVER_LIBS
            /usr/local/lib/librtaudio.so
    )
endif()

set(PROJECT_SOURCES
        ${PLATFORM_SOURCES}
        main.cpp
        mainwindow.h mainwindow.cpp
        mainwindow-1024x600.ui
        resources.qrc
        SampleTypes.h

        dsp/utils/FilterCoefficients.h
        dsp/utils/FftData.h
        dsp/pipeline/oscillators/Oscillator.h dsp/pipeline/oscillators/Oscillator.cpp
        dsp/utils/constants.h
        dsp/utils/PingPongBuffers.h
        dsp/utils/VectorAssembler.h
        dsp/utils/pocketfft/pocketfft_hdronly.h
        radio/receiver/IqReceiver.h radio/receiver/IqReceiver.cpp
        dsp/pipeline/IqPipelineStage.h
        dsp/pipeline/oscillators/OscillatorStage.h
        dsp/pipeline/oscillators/OscillatorMixer.h
        dsp/pipeline/oscillators/OscillatorInjector.h
        dsp/pipeline/filters/FastFIR.h
        dsp/pipeline/correction/DcShift.h
        dsp/pipeline/demodulators/Demodulator.h
        dsp/pipeline/demodulators/AmDemodulator.h

        io/control/device/usb/UsbControl.h
        io/control/device/usb/UsbException.cpp
        io/control/device/usb/UsbException.h
        io/control/device/usb/LibUsbControl.cpp
        io/control/device/usb/LibUsbControl.h
        io/control/device/usb/HidUsbControl.cpp
        io/control/device/usb/HidUsbControl.h
        io/control/device/usb/UsbControl.cpp
        io/control/ControlBase.h
        io/control/device/FunCubeDongle/FunCubeDongle.cpp
        io/control/device/FunCubeDongle/FunCubeDongle.h
        io/control/ControlException.h
        settings/RfSettings.h
        settings/IfSettings.h
        settings/ReceiverSettings.h
        settings/RadioSettings.h
        settings/TransmitterSettings.h

        dsp/pipeline/demodulators/AmDemodulator.cpp
        dsp/pipeline/filters/kernels/FirKernel.h
        dsp/pipeline/filters/kernels/BandPassFirKernel.h
        dsp/pipeline/filters/kernels/BandPassFirKernel.cpp
        dsp/pipeline/filters/kernels/BandStopFirKernel.cpp
        dsp/pipeline/filters/kernels/BandStopFirKernel.h
        dsp/pipeline/filters/kernels/LowPassFirKernel.cpp
        dsp/pipeline/filters/kernels/LowPassFirKernel.h
        SignalEmitter.h
        dsp/pipeline/filters/OverlapBuffers.h
        dsp/utils/window.h
        dsp/utils/OverlapBuffers.h
        dsp/pipeline/decimator/Decimator.h
        dsp/pipeline/decimator/MyFirCoefficients.h

        dsp/pipeline/monitoring/MonitoringStage.h
        config/RadioConfig.h
        config/ReceiverConfig.h
        config/TransmitterConfig.h
        config/AudioConfig.h
        config/ConfigBase.h
        config/ConfigException.h


        io/audio/AudioInput.h

        io/audio/AudioException.h
        io/audio/AudioException.cpp
        dsp/iq/PingPongBufferSink.h
        io/audio/AudioIo.h
        io/audio/AudioOutput.h

        dsp/iq/IqSink.h
        radio/receiver/ReceiverIqEvent.h
        radio/receiver/ReceiverIqEvent.cpp
        radio/receiver/ReceiverAudioEvent.cpp
        radio/receiver/ReceiverAudioEvent.h
        radio/transmitter/IqTransmitter.cpp
        radio/transmitter/IqTransmitter.h
        radio/transmitter/TransmitterAudioEvent.cpp
        radio/transmitter/TransmitterAudioEvent.h
        io/control/ControlSinkFactory.h
        io/control/ControlSinkFactory.cpp

        settings/ReceiverSettingsSink.h
        io/control/PttSink.h
        radio/Radio.cpp
        radio/Radio.h
        settings/RadioSettingsSink.h
        config/ControlConfig.h
        io/control/ControlSink.cpp
        io/control/ControlSink.h
        settings/RadioSettingsSource.h
        io/control/ControlSource.h
        io/control/PttSource.h
        io/control/ControlSourceFactory.cpp
        io/control/ControlSourceFactory.h

        io/control/device/gpio/Gpio.cpp
        io/control/device/gpio/Gpio.h
        io/control/device/gpio/GpioException.cpp
        io/control/device/gpio/GpioException.h
        io/control/device/gpio/digital/DigitalInputLinesRequest.cpp
        io/control/device/gpio/digital/DigitalInputLinesRequest.h
        io/control/device/gpio/digital/GpioRotaryEncoder.cpp
        io/control/device/gpio/digital/GpioRotaryEncoder.h
        io/control/device/gpio/digital/DigitalInputs.cpp
        io/control/device/gpio/digital/DigitalInputs.h
        settings/SettingUpdatePath.h
        settings/SettingUpdate.h
        settings/SettingsException.h
        settings/SettingsBase.h
        util/StringUtils.cpp
        util/StringUtils.h
        io/control/RadioControl.cpp
        io/control/RadioControl.h
        dsp/pipeline/demodulators/FmDemodulator.cpp
        dsp/pipeline/demodulators/FmDemodulator.h
        io/control/device/gpio/digital/DigitalInput.h
        io/control/device/gpio/digital/DigitalInput.cpp
        io/control/device/gpio/digital/DigitalInputFactory.cpp
        io/control/device/gpio/digital/DigitalInputFactory.h
        io/control/device/gpio/digital/DigitalInputFactory.cpp
        config/VariantConfig.h
        config/ConfigFactory.cpp
        config/ConfigFactory.h
        config/DigitalInputsConfig.h
        config/RotaryEncoderConfig.h
        config/DigitalInputConfig.h
        config/FunCubeConfig.h
        settings/RadioSettingsEvent.cpp
        settings/RadioSettingsEvent.h
        settings/SettingUpdateEvent.cpp
        settings/SettingUpdateEvent.h
        io/control/device/gpio/GpioLines.cpp
        io/control/device/gpio/GpioLines.h
        config/GpioLinesConfig.h
        dsp/pipeline/demodulators/SsbDemodulator.cpp
        dsp/pipeline/demodulators/SsbDemodulator.h
        settings/Mode.h
        settings/ModeSettings.h
        settings/ModeSettings.cpp
        dsp/pipeline/resampler/Resampler.h
        settings/TransmitterSettingsSink.h
        dsp/utils/HilbertTransform.h
        dsp/pipeline/modulators/Modulator.h
        dsp/pipeline/modulators/SsbModulator.h
        dsp/iq/IqSource.h
        dsp/iq/AudioIqSource.h
        dsp/iq/IqSourceFactory.cpp
        dsp/iq/IqSourceFactory.h
        config/AudioIqSourceConfig.h
        dsp/iq/AudioSignalIqSource.h
        io/audio/AudioSink.h
        config/AudioSignalIqSourceConfig.h
        dsp/pipeline/IqPipeline.h
        dsp/pipeline/IqRxPipeline.h
        dsp/pipeline/IqRxPipeline.cpp
        dsp/pipeline/IqPipeline.cpp
        dsp/pipeline/IqTxPipeline.cpp
        dsp/pipeline/IqTxPipeline.h
        config/IqIoConfig.h
        io/iq/IqIo.cpp
        io/iq/IqIo.h
        radio/transmitter/TransmitterIqEvent.cpp
        radio/transmitter/TransmitterIqEvent.h
        io/control/device/gpio/digital/DigitalOutput.cpp
        io/control/device/gpio/digital/DigitalOutput.h
        config/DigitalOutputConfig.h
        io/control/device/gpio/digital/DigitalOutputLinesRequest.h
        io/control/device/gpio/digital/DigitalOutputLinesRequest.cpp
        dsp/pipeline/modulators/CwModulator.h
        dsp/pipeline/demodulators/CwDemodulator.cpp
        dsp/pipeline/demodulators/CwDemodulator.h
        dsp/pipeline/correction/IqCorrection.h
        settings/IqCorrectionSettings.h
        settings/MicSettings.h
        dsp/pipeline/oscillators/TwoToneSignalStage.h
        settings/TestSettings.h
        settings/TwoToneSettings.h
        config/BandSelectorConfig.h
        config/BandSelectorLookupItemConfig.h
        io/control/device/gpio/digital/BandSelector.cpp
        io/control/device/gpio/digital/BandSelector.h
        io/audio/drivers/AudioDriver.h
        io/audio/drivers/AudioInputDriver.h
        io/audio/drivers/AudioOutputDriver.h
        dsp/pipeline/modulators/FmModulator.h
        ui/qt/QtChartTheme.h
        ui/qt/QtBandDialog.cpp
        ui/qt/QtBandDialog.h
        settings/Band.h
        settings/BandCategory.h
        settings/Bands.h
        settings/Bands.cpp
        settings/PipelineSettings.h
        settings/RxPipelineSettings.h
        settings/BandSettings.h
        settings/TxPipelineSettings.h
        settings/RadioAndBandSettingsSink.h
        settings/SettingUpdateSink.h
        settings/SettingUpdateSource.h
        settings/SettingValue.h
        settings/Setting.h
        settings/SettingRegistry.h
        ui/qt/QtChartBase.cpp
        ui/qt/QtChartBase.h
        ui/qt/QtTimeSeriesChart.cpp
        ui/qt/QtTimeSeriesChart.h
        ui/qt/QtPanadapter.cpp
        ui/qt/QtPanadapter.h

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

if(USE_PIGPIO)
   set(PLATFORM_LIBRARIES
        ${PIGPIO_LIBRARY}
    )
    set(PLATFORM_INCLUDE_DIRS
        ${PIGPIO_INCLUDE_DIR}
    )
elseif(USE_GPIOD)
    set (PLATFORM_LIBRARIES
        PkgConfig::GPIOD
    )
endif()

target_include_directories(cutesdr-vk6hl PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${AUDIO_DRIVER_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    /usr/include/hidapi/
#    ${SAMPLERATE_INCLUDE_DIRS}
    ${LIQUID_INCLUDE_DIR}
    ${PLATFORM_INCLUDE_DIRS}
)
target_link_directories(cutesdr-vk6hl PRIVATE ${DSPL_DIR})

set (COMMON_LIBRARIES
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts

    ${AUDIO_DRIVER_LIBS}
    ${AUDIO_LINK_LIBS}
    ${VOLK_LIBRARIES}
    usb-1.0
    hidapi-libusb
    nlohmann_json::nlohmann_json
#    ${SAMPLERATE_LIBRARIES}
    ${LIQUID_LIBRARY}
)


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
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}"
)

install(TARGETS cutesdr-vk6hl
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cutesdr-vk6hl)
endif()
