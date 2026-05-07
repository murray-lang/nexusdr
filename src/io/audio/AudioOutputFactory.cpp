#include "CrossPlatformTypes.h"
#include "AudioOutputFactory.h"

ResultCode
AudioOutputFactory::create(const Config::IqIo::AudioOutputConfigVariant& config, AudioOutputVariant& output)
{
  if (holds_alternative<Config::Audio::Fields>(config)) {
    output.emplace<AudioOutput>();
    ResultCode rc = get<AudioOutput>(output).configure(get<Config::Audio::Fields>(config));
    if (rc != ResultCode::OK) {
      output.emplace<monostate>();
    }
    return rc;
  }
  return ResultCode::ERR_AUDIO_OUTPUT_UNKNOWN_TYPE;
}