//
// Created by murray on 20/11/25.
//
#include "settings/TxPipelineSettings.h"
#include "IqTxPipeline.h"
#include "settings/ModeSettings.h"
#include <qdebug.h>



#define FFT_SIZE 2048
#define PING_PONG_LENGTH 8192
#define HILBERT_TAPS 127

#define DEFAULT_SAMPLE_RATE 48000

IqTxPipeline::IqTxPipeline(QObject* eventTarget) :
  IqPipeline(eventTarget),
  m_ssbModulator(ModeSettings::getModeByType(Mode::USB), DEFAULT_SAMPLE_RATE),
  m_cwModulator(ModeSettings::getModeByType(Mode::CWU), DEFAULT_SAMPLE_RATE),
  m_fmModulator(ModeSettings::getModeByType(Mode::FMN), DEFAULT_SAMPLE_RATE),
  m_pModulator(nullptr),
  m_resampler(),
  m_ifFilter(FFT_SIZE),
  m_inputSampleRate(0),
  m_outputSampleRate(0),
  m_pMonitoringStage(nullptr),
  m_twoToneSignal()
{
  m_pMonitoringStage = new MonitoringStage(
    eventTarget,
    TransmitterIqEvent::TxIqEvent,
    [this]() { return m_outputSampleRate; }
  );

  m_twoToneSignal.setEnabled(false); // For now
  
  // addStage(m_pMonitoringStage);
  addStage(&m_ifFilter);
  addStage(&m_resampler);
  // addStage(m_pMonitoringStage);
  addStage(&m_oscillatorMixer);
  addStage(m_pMonitoringStage);
  addStage(&m_iqCorrection);
  
  // addStage(m_pMonitoringStage);
}

IqTxPipeline::~IqTxPipeline()
{
  delete m_pMonitoringStage;
}

void
IqTxPipeline::initialise(IqIo* pIo, AudioSink* pAudioSink)
{
  IqPipeline::initialise(pIo, pAudioSink);
  m_inputSampleRate = pIo->getInputSampleRate();
  setModulatorSampleRate(m_inputSampleRate);
  m_twoToneSignal.initialise(m_inputSampleRate);
  uint32_t preferredOutputRate = pIo->getOutputSampleRate();
  setOutputSampleRate(preferredOutputRate);
  m_resampler.configure(m_inputSampleRate, m_outputSampleRate);
}

void
IqTxPipeline::setModulatorSampleRate(uint32_t sampleRate)
{
  m_ssbModulator.setSampleRate(sampleRate);
  m_cwModulator.setSampleRate(sampleRate);
}

void
IqTxPipeline::setOutputSampleRate(uint32_t outputSampleRate)
{
  m_outputSampleRate = outputSampleRate;
  m_oscillatorMixer.initialise(outputSampleRate, 0);
}

uint32_t
IqTxPipeline::getMaxFramesPerInputPacket() const
{
  uint32_t outputMaxFrames = getMaxFramesPerOutputPacket();
  return outputMaxFrames * m_inputSampleRate / m_outputSampleRate;
}

uint32_t
IqTxPipeline::getMaxFramesPerOutputPacket() const
{
  return m_buffers.getSize() / 2; // 2 is the number of channels (I + Q)
}

void
IqTxPipeline::apply(const TransmitterSettings& settings)
{
  std::lock_guard<std::mutex> lock(m_settingsMutex);
  if (settings.changed & TransmitterSettings::CORRECTION) {
    m_iqCorrection.apply(settings.correctionSettings);
  }
  if (settings.changed & TransmitterSettings::MIC) {
    if (settings.micSettings.changed & MicSettings::GAIN) {
      m_ssbModulator.setInputGain(settings.micSettings.gain);
      qDebug() << "IqTxPipeline::apply(): set SSB modulator input gain to" << settings.micSettings.gain;
    }
  }
}

void
IqTxPipeline::apply(const TxPipelineSettings* settings)
{
  if (settings != nullptr) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    if (settings->changed & PipelineSettings::RF) {
      if (settings->rfSettings.changed & RfSettings::OFFSET) {
        m_oscillatorMixer.setFrequency(settings->rfSettings.offset);
      }
    }
    if (settings->changed & PipelineSettings::MODE) {
      setMode(settings->mode);
    }
  }
}

void
IqTxPipeline::ptt(bool on)
{
  if (on) {
    m_resampler.initialise();
    // m_lastTime =  std::chrono::steady_clock::now();
  }
};

void IqTxPipeline::setMode(const Mode& mode)
{

  IqPipeline::setMode(mode);
  m_ifFilter.getKernel().configure(mode.getLoCut(), mode.getHiCut(), mode.getOffset(), m_inputSampleRate * 2);
  setModulator(mode);
}

void
IqTxPipeline::setModulator(const Mode& mode)
{

  switch (mode.getType()) {
  case Mode::Type::AMN:
  case Mode::Type::AMW:
    m_pModulator = nullptr;
    break;
  case Mode::Type::FMN:
  case Mode::Type::FMW:
    m_pModulator = &m_fmModulator;
    break;
  case Mode::Type::USB:
  case Mode::Type::LSB:
    m_pModulator = &m_ssbModulator;
    break;
  case Mode::Type::CWU:
  case Mode::Type::CWL:
    m_pModulator = &m_cwModulator;
    break;
  default:
    m_pModulator = nullptr;
    throw SettingsException("Unknown mode type");
    break;
  }
  if (m_pModulator != nullptr) {
    m_pModulator->setMode(mode);
  }
}


uint32_t
IqTxPipeline::sinkIq(const vsdrcomplex& samples, uint32_t length)
{
  // auto now = std::chrono::steady_clock::now();
  // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastTime).count();
  // m_lastTime = now;
  // qDebug() << "IqTxPipeline::sinkIq(): received" << length << "samples in" << duration << "ms";

  uint32_t outputLength = 0;
  std::lock_guard<std::mutex> lock(m_settingsMutex);
  if (m_pModulator) {
    if (m_twoToneSignal.getEnabled()) {
      outputLength = m_twoToneSignal.processSamples(samples, m_buffers.input(), length);
      // outputLength = m_pModulator->processSamples(m_buffers.input(), m_buffers.output(), length);
      // m_buffers.flip();
    } else {
      outputLength = m_pModulator->processSamples(samples, m_buffers.input(), length);
    }
  }
  if (outputLength > 0) {
    for (auto stage : m_stages) {
      outputLength = stage->processSamples(m_buffers, outputLength);
      m_buffers.flip();
    }
  }
  //  outputLength = length;
  if (outputLength > 0 && m_pAudioOutSink != nullptr) {
    uint32_t numReals = interleaveComplexToReal(m_buffers.input(), m_audioBuffer, outputLength);
    // uint32_t numReals = interleaveComplexToReal(samples, m_audioBuffer, outputLength);

    m_pAudioOutSink->sinkAudio(m_audioBuffer, numReals, 2);
  }
  return outputLength;
}

uint32_t
IqTxPipeline::interleaveComplexToReal(const vsdrcomplex& vcomplex, vsdrreal& vreal, uint32_t numComplexes)
{
  vreal.resize(numComplexes * 2);

  constexpr bool swapIQ = false;

  const auto* src = vcomplex.data();
  auto* dst = vreal.data();

  if constexpr (swapIQ) {
    for (uint32_t i = 0; i < numComplexes; ++i) {
      dst[2 * i]     = src[i].imag(); // Left = Q
      dst[2 * i + 1] = src[i].real(); // Right = I
    }
  } else {
    for (uint32_t i = 0; i < numComplexes; ++i) {
      dst[2 * i]     = src[i].real(); // Left = I
      dst[2 * i + 1] = src[i].imag(); // Right = Q
    }
  }
  return numComplexes * 2;
}

