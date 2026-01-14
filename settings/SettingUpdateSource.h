//
// Created by murray on 7/1/26.
//

#pragma once

#include "SettingsException.h"

class SettingUpdateSink;
class SettingUpdate;

class SettingUpdateSource
{
public:
  virtual ~SettingUpdateSource() = default;
  virtual void connect(SettingUpdateSink* pSink)
  {
    // This implementation is to avoid the need for a descendant class to implement an empty
    // connect() for this base class.
    throw SettingsException("SingleSettingSource::connect() not implemented.");
  }
protected:
  virtual void notifySettingUpdate(SettingUpdate& settingDelta) = 0;
};
