#ifndef IQAUDIOINPUTDEVICE_H_
#define IQAUDIOINPUTDEVICE_H_

#include <QAudioSource>
#include <QMediaDevices>
#include <QWidget>
#include <QThread>
#include <regex>
//#include "../dsp/IqReceiver.h"
#include "AudioDevice.h"
#include "../../../radio/receiver/IqReceiver.h"
#include "../../../dsp/blocks/Oscillator.h"
#include "../IqSink.h"
#include "../IqSampleCursor.h"
#include "../AudioException.h"
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>


#define PING_PONG_LENGTH 8192

class IqAudioInputDevice : public AudioDevice, public QThread
{

public:
  IqAudioInputDevice(const QAudioFormat &format, IqSink* pSink)
  : AudioDevice(format),
    m_sampleCursor(format),
    m_pSink(pSink),
    m_iqBuffers(PING_PONG_LENGTH),
    m_inputBufferBytes(0),
    m_stopped(false)
  {
    m_inputBufferSize = PING_PONG_LENGTH * format.bytesPerFrame();
    m_inputBuffer.resize(m_inputBufferSize);
  }

  ~IqAudioInputDevice() override = default;

  void start() {
    QThread::start();
    bool opened = open(QIODevice::WriteOnly);
  }
  void stop() {
    m_stopped = true;
    close();
  }

  qint64 readData(char *data, qint64 maxlen) override
  {
    return 0;
  }

  qint64 writeData(const char *data, qint64 length) override
  {
    {
      QByteArray next(data, static_cast<int>(length));
      QMutexLocker locker(&m_mutex);
      if (m_bufferQueue.length() < 4) {
        m_bufferQueue.enqueue(next);
        m_dataAvailable.wakeOne();
      }

    }
    return length;

    // QThread::currentThread()->setPriority(QThread::HighestPriority);
    qint64 bytesRead = 0;
    while (bytesRead < length)
    {
      qint64 remainingBytes = length - bytesRead;
      qint64 bytesToCopy = std::min(remainingBytes, m_inputBufferSize - m_inputBufferBytes);
      std::memcpy(m_inputBuffer.data() + m_inputBufferBytes, data + bytesRead, bytesToCopy);
      bytesRead += bytesToCopy;
      m_inputBufferBytes += bytesToCopy;
      if (m_inputBufferBytes == m_inputBufferSize)
      {
        writeBuffer(m_inputBuffer, m_inputBufferSize);
        m_inputBufferBytes = 0;
      }
    }
    return bytesRead;

  }

  size_t writeBuffer(std::vector<int8_t>& buffer, qint64 length)
  {
    m_iqBuffers.reset();
    m_sampleCursor.reset(buffer.data(), length, false);
    size_t numFrames = length/m_format.bytesPerFrame();

    vsdrcomplex& input = m_iqBuffers.input();
    for (size_t j = 0; j < numFrames; j++) {
      input.at(j) = sdrcomplex(m_sampleCursor.getNormalisedLeft(), m_sampleCursor.getNormalisedRight());
      ++m_sampleCursor;
    }
    m_pSink->sink(m_iqBuffers, static_cast<uint32_t>(numFrames));
    m_iqBuffers.reset();
    return length;
  }

  void run() override
  {
    while (!m_stopped) {
      QByteArray dataChunk;
      {
        QMutexLocker locker(&m_mutex);
        if (m_bufferQueue.isEmpty())
          m_dataAvailable.wait(&m_mutex);
        if (!m_bufferQueue.isEmpty())
          dataChunk = m_bufferQueue.dequeue();
      }
      if (!dataChunk.isEmpty()) {
        qint64 bytesRead = 0;
        qint64 length = dataChunk.length();
        while (bytesRead < length)
        {
          qint64 remainingBytes = length - bytesRead;
          qint64 bytesToCopy = std::min(remainingBytes, m_inputBufferSize - m_inputBufferBytes);
          std::memcpy(m_inputBuffer.data() + m_inputBufferBytes, dataChunk.data() + bytesRead, bytesToCopy);
          bytesRead += bytesToCopy;
          m_inputBufferBytes += bytesToCopy;
          if (m_inputBufferBytes == m_inputBufferSize)
          {
            writeBuffer(m_inputBuffer, m_inputBufferSize);
            m_inputBufferBytes = 0;
          }
        }
      }
    }

  }

private:
  IqSampleCursor m_sampleCursor;
  IqSink* m_pSink;
  ComplexPingPongBuffers m_iqBuffers;
  std::vector<int8_t> m_inputBuffer;
  qint64 m_inputBufferSize;
  qint64 m_inputBufferBytes;
  QQueue<QByteArray> m_bufferQueue;
  QMutex m_mutex;
  QWaitCondition m_dataAvailable;
  bool m_stopped;

};
#endif //IQAUDIOINPUTDEVICE_H_
