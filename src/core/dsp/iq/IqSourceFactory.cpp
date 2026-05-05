//
// Created by murray on 17/11/25.
//

#include "IqSourceFactory.h"

#include "AudioIqSource.h"
#include "AudioSignalIqSource.h"
#include "../../config-settings/config/audio/AudioIqSourceConfig.h"
#include "../../config-settings/config/audio/AudioSignalIqSourceConfig.h"

ResultCode
IqSourceFactory::create(const Config::IqIo::IqSourceConfigVariant& config, IqSourceVariant& source)
{
  ResultCode rc = ResultCode::OK;
  if (holds_alternative<Config::AudioSignalIqSource::Fields>(config)) {
    source.emplace<AudioSignalIqSource>();
    rc = get<AudioSignalIqSource>(source).configure(get<Config::AudioSignalIqSource::Fields>(config));
    if (rc != ResultCode::OK) {
      source.emplace<monostate>();
    }
    return rc;
  }
  if (holds_alternative<Config::AudioIqSource::Fields>(config)) {
    source.emplace<AudioIqSource>();
    rc = get<AudioIqSource>(source).configure(get<Config::AudioIqSource::Fields>(config));
    if (rc != ResultCode::OK) {
      source.emplace<monostate>();
    }
    return rc;
  }
  return ResultCode::ERR_IQ_SOURCE_UNKNOWN_TYPE;
}
