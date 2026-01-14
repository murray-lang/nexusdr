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
#include "settings/RadioSettings.h"
#include "transmitter/IqTransmitter.h"

class Radio : public RadioAndBandSettingsSink, PttSink {

public:
  explicit Radio(QObject *pEventTarget = nullptr);
  ~Radio() override;

  void configure(const RadioConfig* pConfig);
  void start();
  void stop();

  void applySettings(const RadioSettings& settings) override;
  void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  void applySettingUpdate(const SettingUpdate& settingDelta) override;

  void applyBand(const std::string& bandName);

  void applyRfSettings(const RfSettings& settings);
  void applyIfSettings(const IfSettings& settings);

  RadioSettings& getRadioSettings() { return m_settings; }
  const RadioSettings& getRadioSettings() const { return m_settings; }
  BandSettings* getBandSettings(const std::string& bandName);
  const BandSettings* getBandSettings(const std::string& bandName) const;

  const Bands& getBands() const { return m_bands; }

  void ptt(bool on) override;

protected:
  void initialiseBandSettings();
  void pttOn();
  void pttOff();

protected:
  RadioSettings m_settings;
  std::unordered_map<std::string, BandSettings> m_bandSettings;
  mutable std::recursive_mutex m_settingsMutex;
  Bands m_bands;
  IqReceiver* m_pReceiver;
  IqTransmitter* m_pTransmitter;
  RadioControl* m_pControl;

  QObject* m_pEventTarget;
};

