#include "IqReceiver.h"
#include <qdebug.h>
#include <cstdlib>
#include <complex>
#include <sstream>
#include <qcoreapplication.h>

#include "ReceiverAudioEvent.h"
#include "ReceiverIqEvent.h"
#include <config/ReceiverConfig.h>

#define FFT_SIZE 2048


IqReceiver::IqReceiver(QObject* eventTarget) :
  m_mode(),
  m_dcShift(sdrcomplex(0.00447, 0.00348)),
  m_oscillatorMixer(),
  m_decimator(),
  m_afBuffers(PING_PONG_LENGTH),
  m_ifFilter(FFT_SIZE),
  m_afFilter(FFT_SIZE),
  m_amDemodulator(48000),
  m_fmDemodulator(48000),
  m_ssbDemodulator(48000),
  m_pDemodulator(nullptr),
  m_eventTarget(eventTarget),
  m_pIqInput(nullptr),
  m_pAudioOutput(nullptr),
  m_resampleRequired(false)
{
  m_iqStages.push_back(&m_oscillatorMixer);
  m_iqStages.push_back(&m_decimator);
  m_iqStages.push_back(&m_ifFilter);
}

void
IqReceiver::configure(const ReceiverConfig* pConfig)
{
  delete m_pIqInput;
  delete m_pAudioOutput;
  m_pIqInput = nullptr;
  m_pAudioOutput = nullptr;

  auto iqInputConfig = dynamic_cast<const AudioConfig*>(pConfig->getInput());
  m_pIqInput = new AudioInput<sdrcomplex>(this);
  m_pIqInput->initialise(iqInputConfig);

  uint32_t inputSampleRate = m_pIqInput->getSampleRate();
  m_oscillatorMixer.initialise(inputSampleRate, 0);

  auto audioOutputConfig = dynamic_cast<const AudioConfig*>(pConfig->getOutput());
  m_pAudioOutput = new AudioOutput();
  m_pAudioOutput->initialise(audioOutputConfig);

  uint32_t preferredOutputRate = m_pAudioOutput->getSampleRate();
  uint32_t decimatorOutputRate = m_decimator.configure(inputSampleRate, preferredOutputRate);

  if (decimatorOutputRate != preferredOutputRate) {
    m_resampleRequired = true;
    m_resampler.configure(decimatorOutputRate, preferredOutputRate);
  }

  // m_ifFilter.getKernel().configure(
  //   mode.getLoCut(),
  //   mode.getHiCut(),
  //   0.0,
  //   decimatorOutputRate * 2);

  m_afFilter.getKernel().configure(
    100.0,
    3000.0,
    0.0,
    decimatorOutputRate * 2);

  m_amDemodulator.setOutputRate(decimatorOutputRate);
  m_fmDemodulator.setOutputRate(decimatorOutputRate);
  m_ssbDemodulator.setOutputRate(decimatorOutputRate);

  m_ssbDemodulator.setMode(SsbDemodulator::Mode::USB);
}

// void
// IqReceiver::applySettings(const RadioSettings& radioSettings) const
// {
//   //TODO: Audio settings
//   for (auto& deviceController : m_deviceControllers) {
//     deviceController->applySettings(radioSettings);
//   }
// }

void IqReceiver::apply(const ReceiverSettings& settings)
{
  if (settings.changed & ReceiverSettings::RF) {
    if (settings.rfSettings.changed & RfSettings::OFFSET) {
      m_oscillatorMixer.setFrequency(-settings.rfSettings.offset);
    }
  }
  if (settings.changed & ReceiverSettings::MODE) {
    setMode(settings.mode);
  }
}


void
IqReceiver::start() const
{

  if (m_pAudioOutput != nullptr) {
    m_pAudioOutput->start();
  }
  if (m_pIqInput != nullptr) {
    m_pIqInput->start();
  }
}

void
IqReceiver::stop() const
{
  if (m_pIqInput != nullptr) {
    m_pIqInput->stop();
  }
  if (m_pAudioOutput != nullptr) {
    m_pAudioOutput->stop();
  }
}

void IqReceiver::ptt(bool on)
{
  if (on) {
    stop();
  } else {
    start();
  }
}

void
IqReceiver::sink(ComplexPingPongBuffers& buffers, uint32_t inputLength)
{
  // qDebug() << inputLength;
  // emitComplexSignal(SignalEmitter::eSIGNAL_INPUT, buffers.input(), inputLength);
  QCoreApplication::postEvent(m_eventTarget, new ReceiverIqEvent(buffers.input(), inputLength, m_pIqInput->getSampleRate() ));

  uint32_t outputLength = inputLength;

  for (auto stage : m_iqStages) {
    outputLength = stage->processSamples(buffers, outputLength);
    buffers.flip();
  }

  //outputLength = m_amDemodulator.processSamples(buffers.input(), m_afBuffers.input(), outputLength);
  m_demodulatorMutex.lock();
  if (m_pDemodulator != nullptr) {

    if (m_resampleRequired) {
      outputLength = m_pDemodulator->processSamples(buffers.input(), m_afBuffers.input(), outputLength);
      // m_afBuffers.flip();
      outputLength = m_resampler.processSamples(m_afBuffers.input(), m_afBuffers.output(), outputLength);
    } else {
      outputLength = m_pDemodulator->processSamples(buffers.input(), m_afBuffers.output(), outputLength);
    }
    QCoreApplication::postEvent(m_eventTarget, new ReceiverAudioEvent(m_afBuffers.output(), outputLength));
    outputLength = m_pAudioOutput->addAudioData(m_afBuffers.output(), outputLength);
  }
  m_demodulatorMutex.unlock();
}

void
IqReceiver::setMode(const Mode& mode)
{
  m_mode = mode;
  uint32_t decimatorOutputRate = m_decimator.getOutputSampleRate();
  m_ifFilter.getKernel().configure(mode.getLoCut(), mode.getHiCut(), mode.getOffset(), decimatorOutputRate * 2);
  setDemodulator(mode.getType());
}

void
IqReceiver::setDemodulator(Mode::Type modeType)
{
  std::lock_guard<std::mutex> lock(m_demodulatorMutex);
  switch (modeType) {
  case Mode::Type::AMN:
  case Mode::Type::AMW:
    m_pDemodulator = &m_amDemodulator;
    break;
  case Mode::Type::FMN:
  case Mode::Type::FMW:
    m_pDemodulator = &m_fmDemodulator;
    break;
  case Mode::Type::USB:
    m_pDemodulator = &m_ssbDemodulator;
    m_ssbDemodulator.setMode(SsbDemodulator::Mode::USB);
    break;
  case Mode::Type::LSB:
    m_pDemodulator = &m_ssbDemodulator;
    m_ssbDemodulator.setMode(SsbDemodulator::Mode::LSB);
    break;
  default:
    m_pDemodulator = nullptr;
    throw SettingsException("Unknown mode type");
    break;
  }
}
