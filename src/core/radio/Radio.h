//
// Created by murray on 18/08/25.
//

#pragma once
#include <vector>

// #include <io/control/ControlSink.h>
#include <io/control/ControlSource.h>
#include "core/config-settings/config/RadioConfig.h"
#include <io/control/RadioControl.h>

#include "RadioBase.h"
#include "receiver/IqReceiver.h"
#include "core/config-settings/settings/RadioSettingsSink.h"

#include "core/config-settings/settings/ModeSettings.h"
#include "core/config-settings/settings/bands/ActiveBandSettings.h"
#include "core/config-settings/settings/RadioSettings.h"
#include "transmitter/IqTransmitter.h"

class Radio : public RadioBase {

public:
  explicit Radio(EventTarget *pEventTarget = nullptr);
  ~Radio() override;

  void configure(const RadioConfig* pConfig);
  void start();
  void stop();

  void applySettings(const RadioSettings& settings) override;
  // void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  // void applySettingUpdate(SettingUpdate& setting) override;
  // void applySettingUpdates(SettingUpdate* updates, std::size_t count) override;

  void applyBand(const std::string& bandName);

  void split(const std::string& bandA, const std::string& bandB);

  void applyRfSettings(const RfSettings& settings, bool onlyChanged = false)
  {
    m_settings.applyRfSettings(settings, onlyChanged);
  }
  void applyIfSettings(const IfSettings& settings)
  {
    m_settings.applyIfSettings(settings);
  }

  void applyAgcSpeed(AgcSpeed speed);

  // void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);


  const BandSettings* getBandSettings(const std::string& bandName)
  {
    return m_settings.getBandSettings(bandName);
  }
  const BandSettings* getFocusBandSettings()
  {
    return m_settings.getFocusBandSettings();
  }
  const std::string& getFocusBandName()
  {
    return m_settings.getFocusBandName();
  }

  const Bands& getBands()
  {
    return m_settings.getBands();
  }

  void ptt(bool on) override;

protected:
  void pttOn();
  void pttOff();

protected:

  // BandSelector m_bandSelector;
  IqReceiver* m_pReceiver;
  IqTransmitter* m_pTransmitter;
  RadioControl* m_pControl;


};

