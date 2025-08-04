#ifndef IQAUDIODEVICE_H_
#define IQAUDIODEVICE_H_

#include <QAudioSource>
#include <QMediaDevices>
#include <QWidget>
#include <QThread>
#include <regex>
//#include "../dsp/IqReceiver.h"
#include "../../radio/iq/IqReceiver.h"
#include "../../dsp/blocks/Oscillator.h"
#include "IqSink.h"
#include "IqSampleCursor.h"
#include "IqIoException.h"

#define PING_PONG_LENGTH 8192

class IqAudioDevice : public QIODevice
{
  Q_OBJECT

public:
  IqAudioDevice(const QAudioFormat &format, IqSink* pSink)
      : m_format(format),
        m_sampleCursor(format),
        m_pSink(pSink),
        m_iqBuffers(PING_PONG_LENGTH),
        m_inputBufferBytes(0)
  {
    m_inputBufferSize = PING_PONG_LENGTH * format.bytesPerFrame();
    m_inputBuffer.resize(m_inputBufferSize);
  }

  ~IqAudioDevice() override = default;

  void start() {
    bool opened = open(QIODevice::WriteOnly);
  }
  void stop() {
    close();
  }

  qint64 readData(char *data, qint64 maxlen) override
  {
    return 0;
  }

//  qint64 writeData(const char *data, qint64 length) override
//  {
////    QThread::currentThread()->setPriority(QThread::TimeCriticalPriority);
//
//    m_sampleCursor.reset((const int8_t *) data, length, false);
//    size_t numFrames = length/m_format.bytesPerFrame();
//
//    for (size_t j = 0; j < numFrames; j++) {
//      m_pSink->sink(m_sampleCursor.getNormalisedLeft(), m_sampleCursor.getNormalisedRight());
//      ++m_sampleCursor;
//    }
//    return length;
//  }

  qint64 writeData(const char *data, qint64 length) override
  {
//    QThread::currentThread()->setPriority(QThread::HighestPriority);
    size_t bytesRead = 0;
    while (bytesRead < length)
    {
      size_t remainingBytes = length - bytesRead;
      size_t bytesToCopy = std::min(remainingBytes, m_inputBufferSize - m_inputBufferBytes);
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

  size_t writeBuffer(std::vector<int8_t>& buffer, size_t length)
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


  static QAudioDevice findDevice(const std::string& descriptionRegex)
  {
    const QList<QAudioDevice>& devices = QMediaDevices::audioInputs();
    const qsizetype deviceIndex = findDeviceIndex(devices, descriptionRegex);
    return devices.at(deviceIndex);
  }

  static qsizetype findDeviceIndex(const QList<QAudioDevice>& devices, const std::string& descriptionRegex)
  {
    const QList<QAudioDevice>::const_iterator deviceIter = std::find_if(
        devices.begin(), devices.end(),
        [&descriptionRegex](const QAudioDevice& device) -> bool {
          std::basic_regex<char> regex(descriptionRegex, std::regex_constants::ECMAScript | std::regex_constants::icase);
          std::smatch match;
          std::string description = device.description().toStdString();
          return std::regex_search(description, match, regex);
        }
    );
    if (deviceIter == devices.end()) {
      std::ostringstream stringStream;
      stringStream << "An audio device with a name matched by '" << descriptionRegex << "' was not found";
      std::string copyOfStr = stringStream.str();
      throw IqIoException(copyOfStr);
    }
    return std::distance(devices.begin(), deviceIter);
  }

private:
  const QAudioFormat m_format;
  IqSampleCursor m_sampleCursor;
  IqSink* m_pSink;
  ComplexPingPongBuffers m_iqBuffers;
  std::vector<int8_t> m_inputBuffer;
  size_t m_inputBufferSize;
  size_t m_inputBufferBytes;
};
#endif //IQAUDIODEVICE_H_
