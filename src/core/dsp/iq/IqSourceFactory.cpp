//
// Created by murray on 17/11/25.
//

#include "IqSourceFactory.h"

#include "AudioIqSource.h"
#include "AudioSignalIqSource.h"
#include "core/config-settings/config/AudioIqSourceConfig.h"
#include "core/config-settings/config/AudioSignalIqSourceConfig.h"

IqSource*
IqSourceFactory::create(const ConfigBase* pConfig)
{
  IqSource* result = create(pConfig->getType());
  if (result != nullptr) {
    result->configure(pConfig);
  }
  return result;
}

IqSource*
IqSourceFactory::create(const std::string& type)
{
  if (type == AudioIqSourceConfig::type) {
    return new AudioIqSource();
  }
  if (type == AudioSignalIqSourceConfig::type) {
    return new AudioSignalIqSource();
  }
  return nullptr;
}