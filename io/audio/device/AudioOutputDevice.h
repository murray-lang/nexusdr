//
// Created by murray on 18/07/25.
//

#ifndef AUDIOOUTPUTDEVICE_H
#define AUDIOOUTPUTDEVICE_H
#include <QIODevice>
#include <mutex>


#include "AudioDevice.h"
#include "../../../SampleTypes.h"


class AudioOutputDevice : public AudioDevice
{
public:

  AudioOutputDevice(const RtAudio::DeviceInfo& deviceInfo, const Format& format);
  ~AudioOutputDevice() override
  {
    AudioOutputDevice::stop();
  };

  void start() override;
  void stop() override;

  uint32_t addAudioData(const vsdrreal& data, uint32_t length);

  int pullSamples(void *outputBuffer, unsigned int nFrames);

private:
  std::atomic<bool> m_running;

  std::deque<int16_t> m_audioBuffer;
  std::mutex m_mutex;

};

#endif //AUDIOOUTPUTDEVICE_H
