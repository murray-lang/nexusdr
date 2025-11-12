//
// Created by murray on 11/11/25.
//

#include "IqTransmitter.h"

#include <qcoreapplication.h>

#include "TransmitterAudioEvent.h"

#define FFT_SIZE 2048

IqTransmitter::IqTransmitter(QObject* eventTarget) :
  m_mode(),
  m_oscillatorMixer(),
  m_afBuffers(PING_PONG_LENGTH),
  m_ifFilter(FFT_SIZE),
  m_afFilter(FFT_SIZE),
  // m_amDemodulator(48000),
  // m_fmDemodulator(48000),
  // m_ssbDemodulator(48000),
  // m_pDemodulator(nullptr),
  m_eventTarget(eventTarget),
  m_pAudioInput(nullptr),
  m_pIqOutput(nullptr)
{
  // m_iqStages.push_back(&m_oscillatorMixer);
  // m_iqStages.push_back(&m_decimator);
  // m_iqStages.push_back(&m_ifFilter);
}

void
IqTransmitter::configure(const TransmitterConfig* pConfig)
{
  delete m_pAudioInput;
  delete m_pIqOutput;
  m_pAudioInput = nullptr;
  m_pIqOutput = nullptr;

  auto audioInputConfig = dynamic_cast<const AudioConfig*>(pConfig->getInput());
  m_pAudioInput = new AudioInput<sdrreal>(this);
  m_pAudioInput->initialise(audioInputConfig);

  uint32_t inputSampleRate = m_pAudioInput->getSampleRate();
  m_oscillatorMixer.initialise(inputSampleRate, 0);

  auto audioOutputConfig = dynamic_cast<const AudioConfig*>(pConfig->getOutput());
  m_pIqOutput = new AudioOutput();
  m_pIqOutput->initialise(audioOutputConfig);

  uint32_t preferredOutputRate = m_pIqOutput->getSampleRate();
  // uint32_t decimatorOutputRate = m_decimator.configure(inputSampleRate, preferredOutputRate);
  //
  // if (decimatorOutputRate != preferredOutputRate) {
  //   m_resampleRequired = true;
  //   m_resampler.configure(decimatorOutputRate, preferredOutputRate);
  // }

  // m_ifFilter.getKernel().configure(
  //   mode.getLoCut(),
  //   mode.getHiCut(),
  //   0.0,
  //   decimatorOutputRate * 2);

  // m_afFilter.getKernel().configure(
  //   100.0,
  //   3000.0,
  //   0.0,
  //   decimatorOutputRate * 2);
  //
  // m_amDemodulator.setOutputRate(decimatorOutputRate);
  // m_fmDemodulator.setOutputRate(decimatorOutputRate);
  // m_ssbDemodulator.setOutputRate(decimatorOutputRate);
  //
  // m_ssbDemodulator.setMode(SsbDemodulator::Mode::USB);
}

void
IqTransmitter::apply(const TransmitterSettings& settings)
{
}

void
IqTransmitter::start() const
{

  if (m_pAudioInput != nullptr) {
    m_pAudioInput->start();
  }
  if (m_pIqOutput != nullptr) {
    m_pIqOutput->start();
  }
}

void
IqTransmitter::stop() const
{
  if (m_pAudioInput != nullptr) {
    m_pAudioInput->stop();
  }
  if (m_pIqOutput != nullptr) {
    m_pIqOutput->stop();
  }
}

void IqTransmitter::ptt(bool on)
{
  if (on) {
    start();
  } else {
    stop();
  }
}

void
IqTransmitter::sink(RealPingPongBuffers& audioBuffers, uint32_t length)
{
  QCoreApplication::postEvent(m_eventTarget, new TransmitterAudioEvent(audioBuffers.input(), length));
}
