//
// Created by murray on 15/04/23.
//

#pragma once


#include "../../ControlSink.h"
#include "../usb/HidUsbControl.h"
#include "settings/RadioSettings.h"
#include "FCDHidCmd.h"
#include "io/control/ControlException.h"

class FunCubeDongle : public ControlSink {
public:
  FunCubeDongle();
  ~FunCubeDongle() override;

    // void applySettings(const RadioSettings& radioSettings) override;
    // void readSettings(RadioSettings& radioSettings) override;
  void applySettings(const RadioSettings& radioSettings, BandSettings* pBandSettings) override;
  void applySettings(const RadioSettings& radioSettings) override {}
  void applySettingUpdate(SettingUpdate& settingDelta) override
  {
    // throw ControlException("FunCubeDongle::applySingleSetting() not implemented.");
  }

  void ptt(bool on) override;

  // void initialise(const nlohmann::json& json) override;
  void configure(const ConfigBase* pConfig) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;

protected:
  void transactReport(uint8_t buf[65]);
  uint32_t setFrequency(uint32_t freqHz);
  void setRfFilter(TUNERRFFILTERENUM eFilter);
  void setRfFilter(uint32_t freqHz);
  void setIfFilter(TUNERIFFILTERENUM eFilter);
  void setIfFilter(uint32_t bandwidthHz);
  void setLnaGain(float gain);
  void setIfGain(uint8_t ifGain);
  void setIfGain(float ifGain);

protected:
  HidUsbControl m_control;
  float m_lastRfGain;
  float m_lastIfGain;
};
