//
// Created by murray on 4/08/25.
//

#pragma once
#include <rtaudio/RtAudio.h>
#include "../../../../core/config-settings/config/audio/AudioConfig.h"
#include "../AudioDriver.h"


class RtAudioDriver
{
protected:
  ~RtAudioDriver() = default;

public:

  explicit RtAudioDriver(const RtAudio::DeviceInfo& deviceInfo) :
    m_deviceInfo(deviceInfo)
  {
  }

protected:
  RtAudio m_rtAudio;
  RtAudio::DeviceInfo m_deviceInfo;
};
