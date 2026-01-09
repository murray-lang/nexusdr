//
// Created by murray on 7/1/26.
//

#pragma once
#include "RadioSettingsSink.h"

class RadioAndBandSettingsSink : public RadioSettingsSink
{
public:
  virtual void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) = 0;
  // {
  //   throw ControlException("RadioAndBandSettingsSink::applySettings() not implemented.");
  // }
};