#pragma once
#include "ResultCode.h"
#include "AudioOutputTypes.h"
#include "core/config-settings/config/audio/IqIoConfig.h"

class AudioOutputFactory
{
public:
  static ResultCode create(const Config::IqIo::AudioOutputConfigVariant& config, AudioOutputVariant& output);
};
