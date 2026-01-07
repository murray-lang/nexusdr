//
// Created by murray on 18/08/25.
//

#pragma once
#include "SingleSettingSink.h"
#include "io/control/ControlException.h"

// #include "RadioSettings.h"

class RadioSettings;
class SingleSetting;
class BandSettings;

class RadioSettingsSink : public SingleSettingSink
{
public:
  ~RadioSettingsSink() override = default;
  virtual void applySettings(const RadioSettings& settings) = 0;
  // {
  //   throw ControlException("RadioSettingsSink::applySettings() not implemented.");
  // }
  // virtual void applySettings(const RadioSettings& settings, PerBandSettings* pBandSettings) = 0;


};
