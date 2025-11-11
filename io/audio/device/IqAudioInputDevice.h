#ifndef IQAUDIOINPUTDEVICE_H_
#define IQAUDIOINPUTDEVICE_H_

#include <QWidget>
#include <QThread>
#include <regex>
//#include "../dsp/IqReceiver.h"
#include "AudioDevice.h"
#include "../../../radio/receiver/IqReceiver.h"
#include "../../../dsp/blocks/Oscillator.h"
#include "../IqSink.h"
#include "../AudioException.h"
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>


#define PING_PONG_LENGTH 8192

class IqAudioInputDevice : public AudioDevice, public QThread
{

public:
  IqAudioInputDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format, IqSink* pSink)
  : AudioDevice(deviceInfo, format),
    m_running(false),
    // m_sampleCursor(format),
    m_pSink(pSink),
    m_iqBuffers(PING_PONG_LENGTH),
    m_numCurrentSamples(0)
  {
    m_params.deviceId = m_deviceInfo.ID;
    m_params.nChannels = 2;
    m_params.firstChannel = 0;
  }

  ~IqAudioInputDevice() override
  {
    IqAudioInputDevice::stop();
    wait();
  }

  void start() override {
    if (!m_running) {
      unsigned int bufferFrames = PING_PONG_LENGTH;

      RtAudioErrorType rc = m_rtAudio.openStream(
          nullptr,              // no output
          &m_params,            // input params
          m_format.sampleFormat,      // sample format
          m_format.sampleRate,
          &bufferFrames,
          &rtCallback,
          this
      );
      rc = m_rtAudio.startStream();
      m_running = true;
      QThread::start();
    }

  }
  void stop() override {
    if (m_running) {
      m_rtAudio.stopStream();
      m_rtAudio.closeStream();
      m_running = false;
      m_dataAvailable.wakeOne();
    }
  }

  static int rtCallback(void *, void *inputBuffer, unsigned int nframes, double,
                                   RtAudioStreamStatus, void *userData) {
    return static_cast<IqAudioInputDevice*>(userData)->handleCallback(inputBuffer, nframes);
  }


  // size_t getSamples(std::vector<sdrreal>& dest);

  int handleCallback(void *inputBuffer, unsigned int nframes)
  {
    if (inputBuffer) {
      sdrreal* in = static_cast<sdrreal*>(inputBuffer);
      // std::lock_guard<std::mutex> lock(m_mutex);
      QMutexLocker locker(&m_mutex);
      // Append nframes * m_channels samples
      m_iqBuffer.insert(m_iqBuffer.end(), in, in + nframes * 2);
      m_dataAvailable.wakeOne();
    }
    return 0;

  }



  // size_t writeBuffer(std::vector<int8_t>& buffer, qint64 length)
  // {
  //   m_iqBuffers.reset();
  //   m_sampleCursor.reset(buffer.data(), length, false);
  //   size_t numFrames = length/m_format.bytesPerFrame();
  //
  //   vsdrcomplex& input = m_iqBuffers.input();
  //   for (size_t j = 0; j < numFrames; j++) {
  //     input.at(j) = sdrcomplex(m_sampleCursor.getNormalisedLeft(), m_sampleCursor.getNormalisedRight());
  //     ++m_sampleCursor;
  //   }
  //   m_pSink->sink(m_iqBuffers, static_cast<uint32_t>(numFrames));
  //   m_iqBuffers.reset();
  //   return length;
  // }

  void run() override
  {
    while (m_running) {
      vsdrcomplex& input = m_iqBuffers.input();
      {
        QMutexLocker locker(&m_mutex);
        if (m_iqBuffer.empty()) {
          m_dataAvailable.wait(&m_mutex);
        }
        if (!m_iqBuffer.empty()) {
          size_t requiredSamples = m_iqBuffers.current().size() - m_numCurrentSamples;
          size_t numIncomingSamples = m_iqBuffer.size() / 2;
          size_t samplesToRead = std::min(requiredSamples, numIncomingSamples);
          for (size_t i = 0; i < samplesToRead; i++) {
            input.at(i) = sdrcomplex(m_iqBuffer.at(i*2), m_iqBuffer.at(i*2+1));
            // input.at(i) = sdrcomplex(m_iqBuffer.at(i*2+1), m_iqBuffer.at(i*2));
          }
          m_iqBuffer.erase(m_iqBuffer.begin(), m_iqBuffer.begin() + samplesToRead*2);
          m_numCurrentSamples += samplesToRead;
        }
      }
      if (m_numCurrentSamples == input.size()) {
        m_pSink->sink(m_iqBuffers, static_cast<uint32_t>(m_numCurrentSamples));
        m_iqBuffers.reset();
        m_numCurrentSamples = 0;
      }
    }
  }

private:
  std::atomic<bool> m_running;
  // std::mutex m_mutex;
  std::deque<sdrreal> m_iqBuffer;
  RtAudio::StreamParameters m_params;

  // IqSampleCursor m_sampleCursor;
  IqSink* m_pSink;
  ComplexPingPongBuffers m_iqBuffers;
  // std::vector<int8_t> m_inputBuffer;
  // qint64 m_inputBufferSize;
  // qint64 m_inputBufferBytes;
  // QQueue<QByteArray> m_bufferQueue;
  size_t m_numCurrentSamples;
  // QByteArray dataChunk;
  // size_t m_inputBufferSize;
  // std::vector<int8_t> m_inputBuffer;
  QMutex m_mutex;
  QWaitCondition m_dataAvailable;
  // bool m_stopped;

};
#endif //IQAUDIOINPUTDEVICE_H_
