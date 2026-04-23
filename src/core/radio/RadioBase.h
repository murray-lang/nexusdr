//
// Created by murray on 17/3/26.
//

#pragma once

#include "core/config-settings/settings/RadioSettings.h"
#include "core/config-settings/settings/RadioSettingsSink.h"
#include "core/config-settings/config/RadioConfig.h"
#include "core/config-settings/settings/bands/BandSelector.h"
#include "core/config-settings/settings/events/SettingEventBase.h"
#include "core/events/EventTarget.h"
#include "io/control/PttSink.h"
#include "ResultCode.h"

class RadioBase : public RadioSettingsSink, PttSink
{
public:
  explicit RadioBase(EventTarget *pEventTarget = nullptr);
  ~RadioBase() override = default;

  virtual ResultCode configure(const RadioConfig* pConfig) = 0;
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

  void applyRfSettings(const RfSettings& settings, bool onlyChanged = false)
  {
    m_bandSelector.applyRfSettings(settings, onlyChanged);
  }
  void applyIfSettings(const IfSettings& settings)
  {
    m_bandSelector.applyIfSettings(settings);
  }

  void applyBand(const std::string& bandName);

  void split(const std::string& bandA, const std::string& bandB);

  void applyAgcSpeed(AgcSpeed speed);

  // void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);


  const BandSettings* getBandSettings(const std::string& bandName)
  {
    return m_bandSelector.getBandSettings(bandName);
  }
  const BandSettings* getFocusBandSettings()
  {
    return m_settings.getFocusBandSettings();
  }
  std::string getFocusBandName() const
  {
    return m_settings.getFocusBandName();
  }

  const Bands& getBands()
  {
    return m_bandSelector.getAllBands();
  }

protected:
  virtual void notifyUpdate(const SettingUpdate& update, SettingEventBase::EventSource source);

protected:
  BandSelector m_bandSelector;
  EventTarget* m_pEventTarget;
  RadioSettings m_settings;
  uint64_t m_updateSequenceNo;
};
