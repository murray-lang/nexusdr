cmake_minimum_required(VERSION 3.15)
set(SUPPORTED_PLATFORMS "linux" "rpi" "stm32h745i")

set(CMAKE_VERBOSE_MAKEFILE ON)

project(nexusdr VERSION 0.1 LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

if (TARGET_PLATFORM STREQUAL "stm32h745i" )
    add_definitions(
            -DUSE_ETL_COLLECTIONS
            -DMAX_DIGITAL_INPUTS=8
            -DMAX_CONTROL_SINKS=4
            -DMAX_CONTROL_SOURCES=4
            -DMAX_ID_LENGTH=16
            -DMAX_SETTING_DEPTH=8
            -DMAX_SETTING_PATH_LENGTH=96
    )
endif()

set(APP_DIR ${CMAKE_SOURCE_DIR}/src/app)
set(CONFIG_SETTINGS_DIR ${CMAKE_SOURCE_DIR}/src/core/config-settings)
set(CONFIG_DIR ${CONFIG_SETTINGS_DIR}/config)
set(SETTINGS_DIR ${CONFIG_SETTINGS_DIR}/settings)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/src/third-party)
set(ETL_DIR ${THIRD_PARTY_DIR}/etl)
set(ARDUINO_JSON_DIR ${THIRD_PARTY_DIR}/ArduinoJson/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)


set (CONFIG_SOURCES
        ${CONFIG_DIR}/RadioConfig.h
        ${CONFIG_DIR}/RadioConfig.cpp
        ${CONFIG_DIR}/receiver/ReceiverConfig.h
        ${CONFIG_DIR}/receiver/ReceiverConfig.cpp
        ${CONFIG_DIR}/transmitter/TransmitterConfig.h
        ${CONFIG_DIR}/transmitter/TransmitterConfig.cpp
        ${CONFIG_DIR}/audio/AudioConfig.h
        ${CONFIG_DIR}/audio/AudioConfig.cpp
        ${CONFIG_DIR}/base/ConfigBase.h
        ${CONFIG_DIR}/base/ConfigBase.cpp
        ${CONFIG_DIR}/control/ControlConfig.h
        ${CONFIG_DIR}/control/ControlConfig.cpp
        ${CONFIG_DIR}/control/DigitalInputsConfig.h
        ${CONFIG_DIR}/control/DigitalInputsConfig.cpp
        ${CONFIG_DIR}/control/RotaryEncoderConfig.h
        ${CONFIG_DIR}/control/RotaryEncoderConfig.cpp
        ${CONFIG_DIR}/control/DigitalInputConfig.h
        ${CONFIG_DIR}/control/DigitalInputConfig.cpp
        ${CONFIG_DIR}/control/FunCubeConfig.h
        ${CONFIG_DIR}/control/FunCubeConfig.cpp
        ${CONFIG_DIR}/control/GpioLinesConfig.h
        ${CONFIG_DIR}/control/GpioLinesConfig.cpp
        ${CONFIG_DIR}/audio/AudioIqSourceConfig.h
        ${CONFIG_DIR}/audio/AudioIqSourceConfig.cpp
        ${CONFIG_DIR}/audio/AudioSignalIqSourceConfig.h
        ${CONFIG_DIR}/audio/AudioSignalIqSourceConfig.cpp
        ${CONFIG_DIR}/audio/IqIoConfig.h
        ${CONFIG_DIR}/audio/IqIoConfig.cpp
        ${CONFIG_DIR}/control/DigitalOutputConfig.h
        ${CONFIG_DIR}/control/DigitalOutputConfig.cpp
        ${CONFIG_DIR}/control/BandSelectorConfig.h
        ${CONFIG_DIR}/control/BandSelectorConfig.cpp
        ${CONFIG_DIR}/control/BandSelectorConfig.cpp
        ${CONFIG_DIR}/control/BandSelectorBandsConfig.h
        ${CONFIG_DIR}/control/BandSelectorBandsConfig.cpp
        ${CONFIG_DIR}/ui/UiConfig.h
        ${CONFIG_DIR}/ui/UiConfig.cpp
)

set(SETTINGS_SOURCES
        ${SETTINGS_DIR}/RfSettings.h
        ${SETTINGS_DIR}/IfSettings.h
        ${SETTINGS_DIR}/ReceiverSettings.h
        ${SETTINGS_DIR}/RadioSettings.h
        ${SETTINGS_DIR}/TransmitterSettings.h
        ${SETTINGS_DIR}/ReceiverSettingsSink.h
        ${SETTINGS_DIR}/RadioSettingsSink.h
        ${SETTINGS_DIR}/base/SettingUpdatePath.h
        ${SETTINGS_DIR}/base/SettingUpdate.h
        ${SETTINGS_DIR}/base/SettingsException.h
        ${SETTINGS_DIR}/base/SettingsBase.h
        ${SETTINGS_DIR}/RadioSettingsSource.h
        ${SETTINGS_DIR}/Mode.h
        ${SETTINGS_DIR}/ModeSettings.h
        ${SETTINGS_DIR}/ModeSettings.cpp
        ${SETTINGS_DIR}/TransmitterSettingsSink.h
        ${SETTINGS_DIR}/IqCorrectionSettings.h
        ${SETTINGS_DIR}/MicSettings.h
        ${SETTINGS_DIR}/TestSettings.h
        ${SETTINGS_DIR}/TwoToneSettings.h
        ${SETTINGS_DIR}/bands/Band.h
        ${SETTINGS_DIR}/bands/BandCategory.h
        ${SETTINGS_DIR}/bands/Bands.h
        ${SETTINGS_DIR}/bands/Bands.cpp
        ${SETTINGS_DIR}/bands/BandSelector.h
        ${SETTINGS_DIR}/bands/BandSelector.cpp
        ${SETTINGS_DIR}/bands/BandSettings.h
        ${SETTINGS_DIR}/bands/BandSettings.cpp
        ${SETTINGS_DIR}/bands/ActiveBandSettings.h
        ${SETTINGS_DIR}/bands/ActiveBandSettings.cpp
        ${SETTINGS_DIR}/pipeline/PipelineSettings.h
        ${SETTINGS_DIR}/pipeline/RxPipelineSettings.h
        ${SETTINGS_DIR}/pipeline/TxPipelineSettings.h
        ${SETTINGS_DIR}/base/SettingUpdateSink.h
        ${SETTINGS_DIR}/base/SettingUpdateSource.h
        ${SETTINGS_DIR}/base/SettingValue.h
        ${SETTINGS_DIR}/base/Setting.h
        ${SETTINGS_DIR}/base/SettingRegistry.h
        ${SETTINGS_DIR}/pipeline/PipelineId.h
        ${SETTINGS_DIR}/bands/SplitBandId.h
        ${SETTINGS_DIR}/base/SteppableSetting.h
        ${SETTINGS_DIR}/RadioSettings.cpp
        ${SETTINGS_DIR}/AgcSpeed.h
        ${SETTINGS_DIR}/base/SettingUpdateHelpers.cpp
        ${SETTINGS_DIR}/base/SettingUpdateHelpers.h
)

set (DSP_SOURCES
        src/core/dsp/iq/PingPongBufferSink.h
        src/core/dsp/iq/IqSink.h
        src/core/dsp/iq/IqSource.h
        src/core/dsp/iq/AudioIqSource.h
        src/core/dsp/iq/IqSourceFactory.cpp
        src/core/dsp/iq/IqSourceFactory.h
        src/core/dsp/iq/AudioSignalIqSource.h
        src/core/dsp/pipeline/oscillators/Oscillator.h
        src/core/dsp/pipeline/oscillators/Oscillator.cpp
        src/core/dsp/pipeline/IqPipelineStage.h
        src/core/dsp/pipeline/oscillators/OscillatorStage.h
        src/core/dsp/pipeline/oscillators/OscillatorMixer.h
        src/core/dsp/pipeline/oscillators/OscillatorInjector.h
        src/core/dsp/pipeline/filters/FastFIR.h
        src/core/dsp/pipeline/correction/DcShift.h
        src/core/dsp/pipeline/demodulators/Demodulator.h
        src/core/dsp/pipeline/demodulators/AmDemodulator.h
        src/core/dsp/pipeline/demodulators/AmDemodulator.cpp
        src/core/dsp/pipeline/filters/kernels/FirKernel.h
        src/core/dsp/pipeline/filters/kernels/BandPassFirKernel.h
        src/core/dsp/pipeline/filters/kernels/BandPassFirKernel.cpp
        src/core/dsp/pipeline/filters/kernels/BandStopFirKernel.cpp
        src/core/dsp/pipeline/filters/kernels/BandStopFirKernel.h
        src/core/dsp/pipeline/filters/kernels/LowPassFirKernel.cpp
        src/core/dsp/pipeline/filters/kernels/LowPassFirKernel.h
        src/core/dsp/pipeline/filters/OverlapBuffers.h
        src/core/dsp/pipeline/decimator/Decimator.h
        src/core/dsp/pipeline/decimator/MyFirCoefficients.h
        src/core/dsp/pipeline/monitoring/MonitoringStage.h
        src/core/dsp/pipeline/demodulators/FmDemodulator.cpp
        src/core/dsp/pipeline/demodulators/FmDemodulator.h
        src/core/dsp/pipeline/demodulators/SsbDemodulator.cpp
        src/core/dsp/pipeline/demodulators/SsbDemodulator.h
        src/core/dsp/pipeline/resampler/Resampler.h
        src/core/dsp/pipeline/modulators/Modulator.h
        src/core/dsp/pipeline/modulators/SsbModulator.h
        src/core/dsp/pipeline/IqPipeline.h
        src/core/dsp/pipeline/IqRxPipeline.h
        src/core/dsp/pipeline/IqRxPipeline.cpp
        src/core/dsp/pipeline/IqPipeline.cpp
        src/core/dsp/pipeline/IqTxPipeline.cpp
        src/core/dsp/pipeline/IqTxPipeline.h
        src/core/dsp/pipeline/modulators/CwModulator.h
        src/core/dsp/pipeline/demodulators/CwDemodulator.cpp
        src/core/dsp/pipeline/demodulators/CwDemodulator.h
        src/core/dsp/pipeline/correction/IqCorrection.h
        src/core/dsp/pipeline/oscillators/TwoToneSignalStage.h
        src/core/dsp/pipeline/modulators/FmModulator.h
        src/core/dsp/pipeline/RxPipelineAudioTap.cpp
        src/core/dsp/pipeline/RxPipelineAudioTap.h
        src/core/dsp/pipeline/SMeter/SMeterStage.h
        src/core/dsp/pipeline/agc/IqAgcStage.h
        src/core/dsp/utils/window.h
        src/core/dsp/utils/FilterCoefficients.h
        src/core/dsp/utils/FftData.h
        src/core/dsp/utils/constants.h
        src/core/dsp/utils/PingPongBuffers.h
        src/core/dsp/utils/VectorAssembler.h
        src/core/dsp/utils/pocketfft/pocketfft_hdronly.h
        src/core/dsp/utils/HilbertTransform.h
)

set(IO_SOURCES
        ${USB_API_SOURCES}
        ${GPIO_SOURCES}
        src/io/audio/AudioInput.h
        src/io/audio/AudioException.h
        src/io/audio/AudioException.cpp
        src/io/audio/AudioIo.h
        src/io/audio/AudioOutput.h
        src/io/audio/AudioSink.h
        src/io/audio/drivers/AudioDriver.h
        src/io/audio/drivers/AudioInputDriver.h
        src/io/audio/drivers/AudioOutputDriver.h
        src/io/control/ControlBase.h
        src/io/control/ControlSink.h
        src/io/control/ControlSource.h
        src/io/control/ControlSourceFactory.cpp
        src/io/control/ControlSourceFactory.h
        src/io/control/ControlSinkFactory.h
        src/io/control/ControlSinkFactory.cpp
        src/io/control/device/usb/UsbControl.h
        src/io/control/device/usb/UsbException.cpp
        src/io/control/device/usb/UsbException.h
        src/io/control/device/usb/LibUsbControl.h
        src/io/control/device/usb/HidUsbControl.h
        src/io/control/device/FunCubeDongle/FunCubeDongle.cpp
        src/io/control/device/FunCubeDongle/FunCubeDongle.h
        src/io/control/PttSink.h
        src/io/control/PttSource.h
        src/io/control/RadioControl.cpp
        src/io/control/RadioControl.h
        src/io/iq/IqIo.cpp
        src/io/iq/IqIo.h
)

set(RADIO_CORE_SOURCES
        src/core/radio/RadioBase.h
        src/core/radio/RadioBase.cpp
        src/core/radio/receiver/IqReceiver.h src/core/radio/receiver/IqReceiver.cpp
        src/core/radio/transmitter/IqTransmitter.cpp
        src/core/radio/transmitter/IqTransmitter.h
        src/core/radio/Radio.cpp
        src/core/radio/Radio.h
)

set(RADIO_PROXY_SOURCES
        src/core/radio/RadioProxy.cpp
        src/core/radio/RadioProxy.h
)

set(EVENT_SOURCES
        src/core/events/EventDispatcher.h
        src/core/events/EventBase.h
        src/core/events/EventTarget.h
        src/core/events/EventType.h
        ${SETTINGS_DIR}/events/RadioSettingsEvent.h
        ${SETTINGS_DIR}/events/SettingUpdateEvent.h
        src/core/radio/receiver/events/ReceiverMeterEvent.h
        src/core/radio/receiver/events/ReceiverIqEvent.h
        src/core/radio/receiver/events/ReceiverAudioEvent.h
        src/core/radio/transmitter/events/TransmitterIqEvent.h
        src/core/radio/transmitter/events/TransmitterAudioEvent.h
)

set(UTIL_SOURCES
        src/core/util/StringUtils.cpp
        src/core/util/StringUtils.h
)

set(GIT_DIR_LOOKUP_POLICY ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR)
add_subdirectory(${ETL_DIR})

include(cmake/platforms/${TARGET_PLATFORM}/${TARGET_PLATFORM}.cmake)
