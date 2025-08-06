//
// Created by murray on 18/07/25.
//

#ifndef AUDIOOUTPUTDEVICE_H
#define AUDIOOUTPUTDEVICE_H
#include <QAudioFormat>
#include <QIODevice>
#include <queue>

#include "AudioDevice.h"
#include "../../../SampleTypes.h"


class AudioOutputDevice : public AudioDevice
{
public:
  explicit AudioOutputDevice(const QAudioFormat &format, qsizetype bufferSize = 2048);

  void start();
  void stop();

  qint64 readData(char *data, qint64 maxlen) override;
  qint64 writeData(const char *data, qint64 len) override;
  qint64 bytesAvailable() const override;
  qint64 size() const override;

  uint32_t addAudioData(const vsdrreal& data, uint32_t length);

private:
  QByteArray m_audioBuffer;
  qsizetype m_bytesAvailable;
};

#endif //AUDIOOUTPUTDEVICE_H
