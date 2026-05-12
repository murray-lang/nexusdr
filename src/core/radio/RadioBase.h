#pragma once

#include "core/config-settings/settings/RadioSettings.h"
#include "core/config-settings/settings/RadioSettingsSink.h"
#include "core/config-settings/config/RadioConfig.h"
#include "core/config-settings/settings/bands/BandSelector.h"
#include "core/config-settings/settings/events/SettingEventBase.h"
#include "core/events/EventTarget.h"
#include "io/control/PttSink.h"
#include "ResultCode.h"
#include <mutex>



class RadioBase : public RadioSettingsSink, public PttSink
{
public:
  explicit RadioBase(EventTarget *pEventTarget = nullptr);
  ~RadioBase() override = default;

  void setEventTarget(EventTarget* pEventTarget);

  virtual ResultCode configure(const Config::Radio::Fields& config) = 0;
  virtual ResultCode start(EventTarget* pEventTarget) = 0;
  virtual void stop() = 0;

  RadioSettings& getRadioSettings() { return m_settings; }
  [[nodiscard]] const RadioSettings& getRadioSettings() const { return m_settings; }

  [[nodiscard]] uint64_t getUpdateSequenceNo() const { return m_updateSequenceNo; }

  // void applySettings(const RadioSettings& settings) override;
  // void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  ResultCode applySettingUpdate(SettingUpdate& setting) override;
  ResultCode applySettingUpdates(SettingUpdate* updates, std::size_t count) override;

  template<typename T>
  void applySetting(const char * dottedString, T value, bool isDelta = false)
  {
    SettingUpdatePath path;
    RadioSettings::getSettingUpdatePath(dottedString, path);
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

  void applyBand(const BandNameString& bandName);

  void split(const BandNameString& bandA, const BandNameString& bandB);

  void applyAgcSpeed(AgcSpeed speed);

  // void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);


  const BandSettings* getBandSettings(const BandNameString& bandName)
  {
    return m_bandSelector.getBandSettings(bandName);
  }
  const BandSettings* getFocusBandSettings()
  {
    return m_settings.getFocusBandSettings();
  }
  [[nodiscard]] BandNameString getFocusBandName() const
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
  std::mutex m_eventTargetMutex;
  RadioSettings m_settings;
  uint64_t m_updateSequenceNo;
};
