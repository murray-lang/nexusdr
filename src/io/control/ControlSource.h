//
// Created by murray on 20/8/25.
//

#pragma once

#include "ControlBase.h"
#include "core/config-settings/settings/RadioSettingsSource.h"

class ControlSource : public ControlBase, public RadioSettingsSource
{
public:
  ControlSource() : ControlBase(), m_pSettingsSink(nullptr)
  {
  }
  ~ControlSource() override = default;

  void connect(RadioSettingsSink* pSink) override
  {
    m_pSettingsSink = pSink;
  }
protected:
  ResultCode notifySettings(const RadioSettings& radioSettings) override
  {
    if (m_pSettingsSink) {
      return m_pSettingsSink->applySettings(radioSettings);
    }
    return ResultCode::OK;
  }

  ResultCode notifySettingUpdate(SettingUpdate& delta) override
  {
    if (m_pSettingsSink) {
      m_pSettingsSink->applySettingUpdate(delta);
    }
    return ResultCode::OK;
  }

protected:
  RadioSettingsSink* m_pSettingsSink;
};
