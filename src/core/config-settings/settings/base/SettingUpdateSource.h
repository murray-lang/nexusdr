#pragma once
#include "ResultCode.h"

class SettingUpdateSink;
class SettingUpdate;

class SettingUpdateSource
{
public:
  virtual ~SettingUpdateSource() = default;
  virtual void connectSettingUpdateSink(SettingUpdateSink& pSink)
  {
    // This implementation is to avoid the need for a descendant class to implement an empty
    // connect() for this base class.
  }
protected:
  virtual ResultCode notifySettingUpdate(SettingUpdate& settingDelta) = 0;
};
