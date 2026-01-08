//
// Created by murray on 5/1/26.
//

#pragma once
#include "IfSettings.h"
#include "PipelineSettings.h"
#include <QDebug>

class RxPipelineSettings : public PipelineSettings
{
public:
  enum Features
  {
    NONE = 0,
    // 0x01 and 0x02 are used by PipelineSettings
    IF = 0x04,
    MUTE = 0x08,
    AGC = 0x10,
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

  void clearChanged() override
  {
    PipelineSettings::clearChanged();
    ifSettings.clearChanged();
  }

  void setAllChanged() override
  {
    PipelineSettings::setAllChanged();
    ifSettings.setAllChanged();
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
    qDebug() << "RxPipelineSettings::applySettings called";
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

    if (PipelineSettings::applySetting(setting, startIndex)) {
      return true;
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

  static bool getFeaturePath(
    const std::vector<std::string>& featureStrings,
    std::vector<uint32_t>& featuresOut,
    size_t startIndex
    )
  {
    if (PipelineSettings::getFeaturePath(featureStrings, featuresOut, startIndex)) {
      return true;
    }

    if (startIndex >= featureStrings.size()) {
      throw SettingsException("Invalid feature path");
    }
    const std::string& key = featureStrings[startIndex];

    using PathFunc = bool(*)(const std::vector<std::string>&, std::vector<uint32_t>&, size_t);
    static const std::map<std::string, std::pair<Features, PathFunc>> dispatch = {
  {"if",   {IF,  &IfSettings::getFeaturePath}},
  {"mute", {MUTE, &SettingsBase::addFeature<MUTE>}},
  {"agc",  {AGC,  &SettingsBase::addFeature<AGC>}}
    };

    if (auto it = dispatch.find(key); it != dispatch.end()) {
      featuresOut.push_back(it->second.first);
      if (startIndex + 1 < featureStrings.size() && it->second.second) {
        if (!it->second.second(featureStrings, featuresOut, startIndex + 1)) {
          featuresOut.pop_back();
          return false;
        }
      }
      return true;
    }
    return false;
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
    return ifSettings.applySetting(setting, index);
  }

  bool applyAgcSetting(const SingleSetting& setting, int index)
  {
    return false;
  }
};
