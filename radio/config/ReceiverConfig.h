//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_RECEIVERCONFIG_H
#define CUTESDR_VK6HL_RECEIVERCONFIG_H
#include "JsonConfig.h"
#include "AudioConfig.h"
#include "ControlConfig.h"

class ReceiverConfig : public JsonConfig
{
  friend class RadioConfig;
public:
  explicit ReceiverConfig() = default;

  ReceiverConfig(const ReceiverConfig& rhs) :
    m_iqInput(rhs.m_iqInput),
    m_audioOutput(rhs.m_audioOutput),
    m_controllers(rhs.m_controllers)
  {
  }

  ReceiverConfig& operator=(const ReceiverConfig& rhs)
  {
    m_iqInput = rhs.m_iqInput;
    m_audioOutput = rhs.m_audioOutput;
    m_controllers = rhs.m_controllers;
    return *this;
  }

  static ReceiverConfig fromJson(const nlohmann::json& json)
  {
    ReceiverConfig result;
    if (json.contains("iqInput")) {
      result.m_iqInput = AudioConfig::fromJson(json["iqInput"]);
    }
    if (json.contains("audioOutput")) {
      result.m_audioOutput = AudioConfig::fromJson(json["audioOutput"]);
    }
    if (json.contains("controllers"))
    {
      for (auto& controller : json["controllers"])
      {
        result.m_controllers.push_back(ControlConfig::fromJson(controller));
      }
    }
    return result;
  }

  [[nodiscard]] const AudioConfig& getIqInput() const { return m_iqInput; }
  [[nodiscard]] const AudioConfig& getAudioOutput() const { return m_audioOutput; }
  [[nodiscard]] const std::vector<ControlConfig>& getControllers() const { return m_controllers; }

protected:
//  std::optional<AudioConfig> m_iqInput;
//  std::optional<AudioConfig> m_audioOutput;
  AudioConfig m_iqInput;
  AudioConfig m_audioOutput;
  std::vector<ControlConfig> m_controllers;
};
#endif //CUTESDR_VK6HL_RECEIVERCONFIG_H
