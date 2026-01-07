//
// Created by murray on 7/1/26.
//

#pragma once

#include "SettingsException.h"

class SingleSettingSink;
class SingleSetting;

class SingleSettingSource
{
public:
  virtual ~SingleSettingSource() = default;
  virtual void connect(SingleSettingSink* pSink)
  {
    // This implementation is to avoid the need for a descendant class to implement an empty
    // connect() for this base class.
    throw SettingsException("SingleSettingSource::connect() not implemented.");
  }
protected:
  virtual void notifySingleSetting(const SingleSetting& settingDelta) = 0;
};
