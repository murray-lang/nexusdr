#ifndef IQAUDIOINPUTDEVICE_H_
#define IQAUDIOINPUTDEVICE_H_

#include <QWidget>
#include <QThread>
#include <regex>
//#include "../dsp/IqReceiver.h"
#include "AudioDevice.h"
#include "../../../radio/receiver/IqReceiver.h"
#include "../../../dsp/blocks/Oscillator.h"
#include "../AudioSink.h"
#include "../AudioException.h"
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>


#define PING_PONG_LENGTH 8192

template<typename T>
class AudioInputDevice : public AudioDevice, public QThread
{

public:
  AudioInputDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format, AudioSink<T>* pSink)
  : AudioDevice(deviceInfo, format),
    m_running(false),
    // m_sampleCursor(format),
    m_pSink(pSink),
    m_buffers(PING_PONG_LENGTH),
    m_numCurrentFrames(0)
  {
    m_params.deviceId = m_deviceInfo.ID;
    m_params.nChannels = format.channelCount;
    m_params.firstChannel = 0;
  }

  ~AudioInputDevice() override
  {
    AudioInputDevice<T>::stop();
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
    return static_cast<AudioInputDevice<T>*>(userData)->handleCallback(inputBuffer, nframes);
  }


  int handleCallback(void *inputBuffer, unsigned int nframes)
  {
    if (inputBuffer) {
      auto* in = static_cast<float*>(inputBuffer);
      // std::lock_guard<std::mutex> lock(m_mutex);
      QMutexLocker locker(&m_mutex);
      // Append nframes * m_channels samples
      m_buffer.insert(m_buffer.end(), in, in + nframes * m_format.channelCount);
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
      std::vector<T>& input = m_buffers.input();
      {
        QMutexLocker locker(&m_mutex);
        if (m_buffer.empty()) {
          m_dataAvailable.wait(&m_mutex);
        }
        if (!m_buffer.empty()) {
          size_t requiredFrames = m_buffers.current().size() - m_numCurrentFrames;
          size_t numIncomingFrames = m_buffer.size() / m_format.channelCount;
          size_t framesToRead = std::min(requiredFrames, numIncomingFrames);
          getSamplesFromBuffer(framesToRead, m_format.channelCount, input);
          // for (size_t i = 0; i < framesToRead; i++) {
          //   input.at(i) = sdrcomplex(m_buffer.at(i*2), m_buffer.at(i*2+1));
          //   // input.at(i) = sdrcomplex(m_iqBuffer.at(i*2+1), m_iqBuffer.at(i*2));
          // }
          m_buffer.erase(m_buffer.begin(), m_buffer.begin() + framesToRead*m_format.channelCount);
          m_numCurrentFrames += framesToRead;
        }
      }
      if (m_numCurrentFrames == input.size()) {
        m_pSink->sink(m_buffers, static_cast<uint32_t>(m_numCurrentFrames));
        m_buffers.reset();
        m_numCurrentFrames = 0;
      }
    }
  }

  template<typename S>
  void getSamplesFromBuffer(size_t numFrames, uint32_t channelCount, std::vector<S>& input)
  {
    for (size_t i = 0; i < numFrames; i++) {
      for (size_t j = 0; j < channelCount; j++) {
         input.at(i*channelCount + j) = m_buffer.at(i*channelCount + j);
       }
    }
  }

private:
  std::atomic<bool> m_running;
  // std::mutex m_mutex;
  std::deque<float> m_buffer;
  RtAudio::StreamParameters m_params;

  // IqSampleCursor m_sampleCursor;
  AudioSink<T>* m_pSink;
  PingPongBuffers<T> m_buffers;
  // std::vector<int8_t> m_inputBuffer;
  // qint64 m_inputBufferSize;
  // qint64 m_inputBufferBytes;
  // QQueue<QByteArray> m_bufferQueue;
  size_t m_numCurrentFrames;
  // QByteArray dataChunk;
  // size_t m_inputBufferSize;
  // std::vector<int8_t> m_inputBuffer;
  QMutex m_mutex;
  QWaitCondition m_dataAvailable;
  // bool m_stopped;

};

template<>
template<>
inline void AudioInputDevice<sdrcomplex>::getSamplesFromBuffer<sdrcomplex>(
  size_t numFrames,
  uint32_t channelCount,
  std::vector<sdrcomplex>& input
) {
  for (size_t i = 0; i < numFrames; i++) {
    input.at(i) = sdrcomplex(m_buffer.at(i*2), m_buffer.at(i*2+1));
  }
}

#endif //IQAUDIOINPUTDEVICE_H_
