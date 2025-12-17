#pragma once

#include <QThread>
#include "RtAudioDriver.h"
// #include "../../../radio/iq/PingPongBufferSink.h"
#include "../../AudioSink.h"
#include <QMutex>
#include <QWaitCondition>
#include <qdebug.h>

#include "io/audio/drivers/AudioInputDriver.h"


#define DEFAULT_BUFFER_SIZE 8192

// template<typename T>
class RtAudioInputDriver : public AudioInputDriver, public RtAudioDriver, public QThread
{

public:
  RtAudioInputDriver(const RtAudio::DeviceInfo& deviceInfo, const Format& format, AudioSink* pSink) :
   AudioInputDriver(format, pSink),
   RtAudioDriver(deviceInfo),
    m_running(false),
    // m_sampleCursor(format),
    m_pSink(pSink),
    m_outputBuffer(DEFAULT_BUFFER_SIZE),
    m_maxPacketFrames(0),
    m_numCurrentFrames(0)
  {
    m_params.deviceId = m_deviceInfo.ID;
    m_params.nChannels = format.channelCount;
    m_params.firstChannel = 0;
  }

  ~RtAudioInputDriver() override
  {
    RtAudioInputDriver::stop();
    wait();
  }

  [[nodiscard]] uint32_t getMaxChannels() const override {
    return m_deviceInfo.inputChannels;
  }

  [[nodiscard]] uint32_t getNumChannels() const override {
    return m_format.channelCount;
  }

  void start(uint32_t maxPacketFrames) override {
    m_maxPacketFrames = maxPacketFrames;
    if (!m_running) {
      // unsigned int bufferFrames = DEFAULT_BUFFER_SIZE;

      RtAudioErrorType rc = m_rtAudio.openStream(
          nullptr,              // no output
          &m_params,            // input params
          m_format.sampleFormat,      // sample format
          m_format.sampleRate,
          &m_maxPacketFrames,
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
    // qDebug() << "rtCallback(): " << nframes << " frames.";
    return static_cast<RtAudioInputDriver*>(userData)->handleCallback(inputBuffer, nframes);
  }


  int handleCallback(void *inputBuffer, unsigned int nframes)
  {
    if (inputBuffer) {
      auto* in = static_cast<float*>(inputBuffer);
      // std::lock_guard<std::mutex> lock(m_mutex);
      QMutexLocker locker(&m_mutex);
      // Append nframes * m_channels samples
      m_queue.insert(m_queue.end(), in, in + nframes * m_format.channelCount);
      m_dataAvailable.wakeOne();
    }
    return 0;

  }

  void run() override
  {
    while (m_running) {
      {
        QMutexLocker locker(&m_mutex);
        if (m_queue.empty()) {
          m_dataAvailable.wait(&m_mutex);
        }
        if (!m_queue.empty()) {
          uint32_t requiredFrames = m_maxPacketFrames - m_numCurrentFrames;
          uint32_t numIncomingFrames = m_queue.size() / m_format.channelCount;
          uint32_t framesToRead = std::min(requiredFrames, numIncomingFrames);
          getSamplesFromBuffer(framesToRead, m_format.channelCount, m_outputBuffer);
          m_queue.erase(m_queue.begin(), m_queue.begin() + framesToRead*m_format.channelCount);
          m_numCurrentFrames += framesToRead;
        }
      }
      if (m_numCurrentFrames == m_maxPacketFrames) {
        // qDebug() << "AudioInputDevice::run(): " << m_numCurrentFrames << " frames.";
        m_pSink->sinkAudio(
          m_outputBuffer,
          static_cast<uint32_t>(m_numCurrentFrames) * m_format.channelCount,
          m_format.channelCount
        );
        m_numCurrentFrames = 0;
      }
    }
  }

  template<typename S>
  void getSamplesFromBuffer(size_t numFrames, uint32_t channelCount, std::vector<S>& input)
  {
    for (size_t i = 0; i < numFrames; i++) {
      for (size_t j = 0; j < channelCount; j++) {
         input.at(i*channelCount + j) = m_queue.at(i*channelCount + j);
       }
    }
  }

private:
  std::atomic<bool> m_running;
  // std::mutex m_mutex;
  std::deque<float> m_queue;
  RtAudio::StreamParameters m_params;

  AudioSink* m_pSink;
  vsdrreal m_outputBuffer;
  uint32_t m_maxPacketFrames;
  uint32_t m_numCurrentFrames;
  QMutex m_mutex;
  QWaitCondition m_dataAvailable;
  // bool m_stopped;

};
