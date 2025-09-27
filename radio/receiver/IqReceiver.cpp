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
  m_dcShift(sdrcomplex(0.00447, 0.00348)),
  m_oscillatorMixer(),
  m_decimator(),
  m_afBuffers(PING_PONG_LENGTH),
  m_ifFilter(FFT_SIZE),
  m_afFilter(FFT_SIZE),
  m_amDemodulator(48000),
  m_fmDemodulator(48000),
  m_eventTarget(eventTarget),
  m_pIqInput(nullptr),
  m_pAudioOutput(nullptr)
{
  m_iqStages.push_back(&m_oscillatorMixer);
  m_iqStages.push_back(&m_decimator);
  m_iqStages.push_back(&m_ifFilter);
}

// IqReceiver::IqReceiver(int32_t sampleRate, size_t fftSize, QObject* eventTarget) :
//   // m_fftThread(fftSize),
//   m_dcShift(sdrcomplex(0.00447, 0.00348)),
//   m_oscillatorMixer(sampleRate, -lo),
//   // m_signal1(sampleRate, lo + 300),
//   // m_signal2(sampleRate, lo + 1200),
//   // m_signal3(sampleRate, lo + 2700),
//   //    m_decimator(sampleRate, 24000, fftSize, 0),
//   m_decimator(sampleRate, 48000),
//   //    m_ifBuffers(PING_PONG_LENGTH),
//   m_afBuffers(PING_PONG_LENGTH),
//   // m_debugOscillator(sampleRate, 32000),
//   m_ifFilter(fftSize),
//   m_afFilter(fftSize),
//   m_pDemodulator(nullptr),
//   // m_timeseriesEmitter("timeseries", MeteringStage::eTIMESERIES, *this, m_fftThread),
//   // m_spectrumEmitter("spectrum", MeteringStage::eSPECTRUM, *this, m_fftThread),
//   m_eventTarget(eventTarget),
//   m_pIqInput(nullptr),
//   m_pAudioOutput(nullptr),
//   m_deviceControllers()
// {
//   // m_iqStages.push_back(&m_spectrumEmitter);
//   m_iqStages.push_back(&m_oscillatorMixer);
//   m_iqStages.push_back(&m_decimator);
//   m_iqStages.push_back(&m_ifFilter);
//
//   uint32_t decimatorOutputRate = m_decimator.getOutputSampleRate();
//
//   m_ifFilter.getKernel().configure(
//     -3500.0,
//     3500.0,
//     0.0,
//     decimatorOutputRate * 2);
//
//   m_afFilter.getKernel().configure(
//     1000.0,
//     1000.0,
//     0.0,
//     decimatorOutputRate * 2);
//
//   m_pDemodulator = new AmDemodulator(decimatorOutputRate);
//
//   // m_fftThread.start();
// }

void
IqReceiver::configure(const ReceiverConfig* pConfig)
{
  delete m_pIqInput;
  delete m_pAudioOutput;
  m_pIqInput = nullptr;
  m_pAudioOutput = nullptr;

  auto iqInputConfig = dynamic_cast<const AudioConfig*>(pConfig->getInput());
  m_pIqInput = new IqAudioInput(this);
  m_pIqInput->initialise(iqInputConfig);

  uint32_t inputSampleRate = m_pIqInput->getSampleRate();
  m_oscillatorMixer.initialise(inputSampleRate, 0);

  auto audioOutputConfig = dynamic_cast<const AudioConfig*>(pConfig->getOutput());
  m_pAudioOutput = new AudioOutput();
  m_pAudioOutput->initialise(audioOutputConfig);

  uint32_t outputSampleRate = m_pAudioOutput->getSampleRate();
  m_decimator.configure(inputSampleRate, outputSampleRate);

  uint32_t decimatorOutputRate = m_decimator.getOutputSampleRate();

  m_ifFilter.getKernel().configure(
    -3500.0,
    3500.0,
    0.0,
    decimatorOutputRate * 2);

  m_afFilter.getKernel().configure(
    1000.0,
    1000.0,
    0.0,
    decimatorOutputRate * 2);

  m_amDemodulator.setOutputRate(decimatorOutputRate);
  m_fmDemodulator.setOutputRate(decimatorOutputRate);
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

//void
//IqReceiver::sink(sdrreal i, sdrreal q)
//{
//  sdrcomplex next(i, q);
//  m_ifBuffers.input()[m_inputCount++] = next;
//
//  if (m_inputCount == PING_PONG_LENGTH) {
//      processSamples(m_ifBuffers, PING_PONG_LENGTH);
//      m_inputCount = 0;
//      m_ifBuffers.reset();
//  }
//}

void IqReceiver::ptt(bool on)
{
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

  outputLength = m_amDemodulator.processSamples(buffers.input(), m_afBuffers.input(), outputLength);

  //   vsdrcomplex audiosamples(outputLength, complexZero);
  //   for (int i = 0; i < outputLength; i++) {
  //     audiosamples.at(i) = sdrcomplex(m_afBuffers.input().at(i), 0.0f);
  // //    audiosamples.at(i) = buffers.output().at(i).real();
  //   }
  // qDebug() << "Have " << outputLength << " audio samples";
  //emitTimeseries(m_afBuffers.input(), outputLength);
  //emitAudioData(m_afBuffers.input(), outputLength);
  // emitRealSignal(SignalEmitter::eSIGNAL_DEMODULATED, m_afBuffers.input(), outputLength);
  QCoreApplication::postEvent(m_eventTarget, new ReceiverAudioEvent(m_afBuffers.input(), outputLength));
  outputLength = m_pAudioOutput->addAudioData(m_afBuffers.input(), outputLength);
  // m_fftThread.add(audiosamples, outputLength, true);
  //const vsdrcomplex& sincPulse = m_afFilter.getKernel().getComplexSincPulse();
}
