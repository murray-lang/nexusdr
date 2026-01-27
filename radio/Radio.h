//
// Created by murray on 18/08/25.
//

#pragma once
#include <vector>

// #include <io/control/ControlSink.h>
#include <io/control/ControlSource.h>
#include "config/RadioConfig.h"
#include <io/control/RadioControl.h>
#include "receiver/IqReceiver.h"
#include <settings/RadioSettingsSink.h>

#include "../settings/ModeSettings.h"
#include "../settings/bands/BandSelector.h"
#include "settings/RadioSettings.h"
#include "transmitter/IqTransmitter.h"

class Radio : public RadioSettingsSink, PttSink {

public:
  explicit Radio(QObject *pEventTarget = nullptr);
  ~Radio() override;

  void configure(const RadioConfig* pConfig);
  void start();
  void stop();

  template<typename T>
  void applySetting(const char * dottedString, T value, bool isDelta = false)
  {
    SettingUpdatePath path = RadioSettings::getSettingUpdatePath(dottedString);
    SettingUpdate update(path, value, isDelta ? SettingUpdate::DELTA : SettingUpdate::VALUE);
    applySettingUpdate(update);
  }
  void applySettings(const RadioSettings& settings) override;
  // void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  void applySettingUpdate(SettingUpdate& setting) override;

  void applyBand(const std::string& bandName);

  void applyRfSettings(const RfSettings& settings, bool onlyChanged = false)
  {
    m_settings.applyRfSettings(settings, onlyChanged);
  }
  void applyIfSettings(const IfSettings& settings)
  {
    m_settings.applyIfSettings(settings);
  }

  // void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);

  RadioSettings& getRadioSettings() { return m_settings; }
  [[nodiscard]] const RadioSettings& getRadioSettings() const { return m_settings; }
  static const BandSettings* getBandSettings(const std::string& bandName)
  {
    return RadioSettings::getBandSettings(bandName);
  }
  static  const BandSettings* getFocusBandSettings()
  {
    return RadioSettings::getFocusBandSettings();
  }
  static const std::string& getFocusBandName()
  {
    return RadioSettings::getFocusBandName();
  }

  static const Bands& getBands()
  {
    return RadioSettings::getBands();
  }

  void ptt(bool on) override;

protected:
  void pttOn();
  void pttOff();

protected:
  RadioSettings m_settings;
  // BandSelector m_bandSelector;
  IqReceiver* m_pReceiver;
  IqTransmitter* m_pTransmitter;
  RadioControl* m_pControl;

  QObject* m_pEventTarget;
};

