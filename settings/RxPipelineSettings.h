//
// Created by murray on 5/1/26.
//

#pragma once
#include "IfSettings.h"
#include "PipelineSettings.h"

class RxPipelineSettings : public PipelineSettings
{
public:
  enum Features
  {
    NONE = 0,
    IF = 0x01,
    MUTE = 0x02,
    AGC = 0x04,
    ALL = static_cast<uint32_t>(~0U)
  };
  RxPipelineSettings() : PipelineSettings(), mute(false)
  {
  }
  // RxPipelineSettings(const RxPipelineSettings& rhs) = default;
  ~RxPipelineSettings() override = default;

  RxPipelineSettings& operator=(const RxPipelineSettings& rhs)
  {
    if (this != &rhs) {
      PipelineSettings::operator=(rhs);
      mute = rhs.mute;
      ifSettings = rhs.ifSettings;
    }
    return *this;
  }

  IfSettings& getIfSettings()
  {
    return ifSettings;
  }

  void applyIfSettings(const IfSettings& settings)
  {
    ifSettings.applySettings(settings);
  }

  bool applySettings(const RxPipelineSettings& settings)
  {
    bool somethingChanged = PipelineSettings::applySettings(settings);
    if (settings.changed & IF) {
      if (ifSettings.applySettings(settings.ifSettings)) {
        changed |= IF;
        somethingChanged = true;
      }
    }
    return somethingChanged;
  }

  bool applySetting(const SingleSetting& setting, int startIndex) override
  {
    if (startIndex >= setting.getPath().getFeatures().size()) {
      throw SettingsException("Invalid setting path");
    }

    uint32_t targetFeature = setting.getPath().getFeatures()[startIndex];
    bool settingChange = false;

    using HandlerFunc = bool (RxPipelineSettings::*)(const SingleSetting&, int);
    struct Handler {
      Features feature;
      HandlerFunc method;
    };

    static constexpr Handler dispatchTable[] = {
  { MUTE,   &RxPipelineSettings::applyMuteSetting },
  { IF,     &RxPipelineSettings::applyIfSettings },
  { AGC,     &RxPipelineSettings::applyAgcSetting },
    };

    for (const auto& h : dispatchTable) {
      if (targetFeature & h.feature) {
        if ((this->*h.method)(setting, startIndex)) {
          settingChange = true;
          changed |= h.feature;
        }
      }
    }
    return settingChange;
  }

  static void getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    )
  {
    PipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex);

    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }
    const std::string& key = featureStrings[startIndex];

    using PathFunc = void(*)(const std::vector<std::string>&, std::vector<uint32_t>&, size_t);
    static const std::map<std::string, std::pair<Features, PathFunc>> dispatch = {
  {"if",   {IF,  &IfSettings::getFeaturePath}},
  {"mute", {MUTE, &SettingsBase::addFeature<MUTE>}},
  {"agc",  {AGC,  &SettingsBase::addFeature<AGC>}}
    };

    if (auto it = dispatch.find(key); it != dispatch.end()) {
      featuresOut.push_back(it->second.first);
      if (startIndex + 1 < featureStrings.size() && it->second.second) {
        it->second.second(featureStrings, featuresOut, startIndex + 1);
      }
    } else {
      throw SettingsException("Unknown RxPipeline setting: " + key);
    }
  }

  bool mute;
  IfSettings ifSettings;

protected:
  bool applyMuteSetting(const SingleSetting& setting, int index)
  {
    bool newMute = std::get<bool>(setting.getValue());
    if (mute != newMute) {
      mute = newMute;
      return true;
    }
    return false;
  }

  bool applyIfSettings(const SingleSetting& setting, int index)
  {
    return ifSettings.applySetting(setting, index + 1);
  }

  bool applyAgcSetting(const SingleSetting& setting, int index)
  {
    return false;
  }
};
