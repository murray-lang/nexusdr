//
// Created by murray on 14/9/25.
//

#include "ConfigFactory.h"

#include "AudioConfig.h"
#include "ReceiverConfig.h"
#include "TransmitterConfig.h"  
#include "VariantConfig.h"
#include <stdexcept>

#include "ControlConfig.h"
#include "DigitalInputGroupConfig.h"
#include "FunCubeConfig.h"
#include "RotaryEncoderConfig.h"
#include "util/StringUtils.h"

ConfigBase*
ConfigFactory::create(const VariantConfig& item)
{
  const std::string& type = item.getType();
  const nlohmann::json& config = item.getConfig();

  return create(type, config);
}

ConfigBase*
ConfigFactory::create(const std::string& type)
{
  std::string typeAsLower = StringUtils::toLowerCase(type);
  if (typeAsLower == ControlConfig::type) {
    auto * result = new ControlConfig();
    return result;
  }
  if (typeAsLower == DigitalInputGroupConfig::type) {
    auto * result = new DigitalInputGroupConfig();
    return result;
  }
  if (typeAsLower == DigitalInputConfig::type) {
    auto * result = new DigitalInputConfig();
    return result;
  }
  if (typeAsLower == RotaryEncoderConfig::type) {
    auto * result = new RotaryEncoderConfig();
    return result;
  }
  if (typeAsLower == ReceiverConfig::type) {
    auto * result = new ReceiverConfig();
    return result;
  }
  if (typeAsLower == TransmitterConfig::type) {
    auto * result = new TransmitterConfig();
    return result;
  }
  if (typeAsLower == AudioConfig::type) {
    auto * result = new AudioConfig();
    return result;
  }
  if (typeAsLower == AudioConfig::type) {
    auto * result = new AudioConfig();
    return result;
  }
  if (typeAsLower == FunCubeConfig::type) {
    auto * result = new FunCubeConfig();
    return result;
  }
  return nullptr;
}

ConfigBase*
ConfigFactory::create(const std::string& type, const nlohmann::json& config)
{
  ConfigBase* result = create(type);
  if (result == nullptr) {
    throw std::runtime_error("Unknown config type: " + type);
  }
  result->initialise(config);
  return result;
}