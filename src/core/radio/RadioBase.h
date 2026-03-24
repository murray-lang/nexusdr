//
// Created by murray on 17/3/26.
//

#pragma once

#include "core/config-settings/settings/RadioSettings.h"
#include "core/config-settings/settings/RadioSettingsSink.h"
#include "core/config-settings/config/RadioConfig.h"
#include "core/events/EventTarget.h"
#include "io/control/PttSink.h"

class RadioBase : public RadioSettingsSink, PttSink
{
public:
  explicit RadioBase(EventTarget *pEventTarget = nullptr);
  ~RadioBase() override = default;

  virtual void configure(const RadioConfig* pConfig) = 0;
  virtual void start() = 0;
  virtual void stop() = 0;

  RadioSettings& getRadioSettings() { return m_settings; }
  [[nodiscard]] const RadioSettings& getRadioSettings() const { return m_settings; }

  [[nodiscard]] uint64_t getUpdateSequenceNo() const { return m_updateSequenceNo; }

  // void applySettings(const RadioSettings& settings) override;
  // void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  void applySettingUpdate(SettingUpdate& setting) override;
  void applySettingUpdates(SettingUpdate* updates, std::size_t count) override;

  template<typename T>
  void applySetting(const char * dottedString, T value, bool isDelta = false)
  {
    SettingUpdatePath path = RadioSettings::getSettingUpdatePath(dottedString);
    SettingUpdate update(path, value, isDelta ? SettingUpdate::DELTA : SettingUpdate::VALUE);
    applySettingUpdate(update);
  }

protected:
protected:
  virtual void notifyUpdate(const SettingUpdate& update);

protected:
  EventTarget* m_pEventTarget;
  RadioSettings m_settings;
  uint64_t m_updateSequenceNo;
};
