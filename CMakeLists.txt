cmake_minimum_required(VERSION 3.15)
set(SUPPORTED_PLATFORMS "linux" "rpi" "stm32h745i")

set(CMAKE_VERBOSE_MAKEFILE ON)

project(nexusdr VERSION 0.1 LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

#if (TARGET_PLATFORM STREQUAL "stm32h745i" )
    add_definitions(
            -DUSE_ETL_COLLECTIONS
            -DMAX_DIGITAL_INPUTS=8
            -DMAX_CONTROL_SINKS=4
            -DMAX_CONTROL_SOURCES=4
            -DMAX_SETTING_PATH_LENGTH=96
    )
#endif()

set(APP_DIR ${CMAKE_SOURCE_DIR}/src/app)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/src/third-party)
set(ETL_DIR ${THIRD_PARTY_DIR}/etl)
set(ARDUINO_JSON_DIR ${THIRD_PARTY_DIR}/ArduinoJson/src)


set (CONFIG_SOURCES
        src/core/config-settings/config/RadioConfig.h
        src/core/config-settings/config/receiver/ReceiverConfig.h
        src/core/config-settings/config/transmitter/TransmitterConfig.h
        src/core/config-settings/config/audio/AudioConfig.h
        src/core/config-settings/config/base/ConfigBase.h
        src/core/config-settings/config/control/ControlConfig.h
        src/core/config-settings/config/control/DigitalInputsConfig.h
        src/core/config-settings/config/control/RotaryEncoderConfig.h
        src/core/config-settings/config/control/DigitalInputConfig.h
        src/core/config-settings/config/control/FunCubeConfig.h
        src/core/config-settings/config/control/GpioLinesConfig.h
        src/core/config-settings/config/audio/AudioIqSourceConfig.h
        src/core/config-settings/config/audio/AudioSignalIqSourceConfig.h
        src/core/config-settings/config/audio/IqIoConfig.h
        src/core/config-settings/config/control/DigitalOutputConfig.h
        src/core/config-settings/config/control/BandSelectorConfig.h
        src/core/config-settings/config/control/BandSelectorBandsConfig.h
        src/core/config-settings/config/ui/UiConfig.h
)

set(SETTINGS_SOURCES
        src/core/config-settings/settings/RfSettings.h
        src/core/config-settings/settings/IfSettings.h
        src/core/config-settings/settings/ReceiverSettings.h
        src/core/config-settings/settings/RadioSettings.h
        src/core/config-settings/settings/TransmitterSettings.h
        src/core/config-settings/settings/ReceiverSettingsSink.h
        src/core/config-settings/settings/RadioSettingsSink.h
        src/core/config-settings/settings/base/SettingUpdatePath.h
        src/core/config-settings/settings/base/SettingUpdate.h
        src/core/config-settings/settings/base/SettingsException.h
        src/core/config-settings/settings/base/SettingsBase.h
        src/core/config-settings/settings/RadioSettingsSource.h

        src/core/config-settings/settings/Mode.h
        src/core/config-settings/settings/ModeSettings.h
        src/core/config-settings/settings/ModeSettings.cpp
        src/core/config-settings/settings/TransmitterSettingsSink.h
        src/core/config-settings/settings/IqCorrectionSettings.h
        src/core/config-settings/settings/MicSettings.h
        src/core/config-settings/settings/TestSettings.h
        src/core/config-settings/settings/TwoToneSettings.h
        src/core/config-settings/settings/bands/Band.h
        src/core/config-settings/settings/bands/BandCategory.h
        src/core/config-settings/settings/bands/Bands.h
        src/core/config-settings/settings/bands/Bands.cpp
        src/core/config-settings/settings/bands/BandSelector.h
        src/core/config-settings/settings/bands/BandSelector.cpp
        src/core/config-settings/settings/bands/BandSettings.h
        src/core/config-settings/settings/bands/BandSettings.cpp
        src/core/config-settings/settings/bands/ActiveBandSettings.h
        src/core/config-settings/settings/bands/ActiveBandSettings.cpp
        src/core/config-settings/settings/pipeline/PipelineSettings.h
        src/core/config-settings/settings/pipeline/RxPipelineSettings.h
        src/core/config-settings/settings/pipeline/TxPipelineSettings.h
        src/core/config-settings/settings/base/SettingUpdateSink.h
        src/core/config-settings/settings/base/SettingUpdateSource.h
        src/core/config-settings/settings/base/SettingValue.h
        src/core/config-settings/settings/base/Setting.h
        src/core/config-settings/settings/base/SettingRegistry.h
        src/core/config-settings/settings/pipeline/PipelineId.h
        src/core/config-settings/settings/bands/SplitBandId.h
        src/core/config-settings/settings/base/SteppableSetting.h
        src/core/config-settings/settings/RadioSettings.cpp
        src/core/config-settings/settings/AgcSpeed.h
        src/core/config-settings/settings/base/SettingUpdateHelpers.cpp
        src/core/config-settings/settings/base/SettingUpdateHelpers.h
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
        src/io/control/ControlException.h
        src/io/control/ControlSink.cpp
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
        src/core/config-settings/settings/events/RadioSettingsEvent.h
        src/core/config-settings/settings/events/SettingUpdateEvent.h
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
