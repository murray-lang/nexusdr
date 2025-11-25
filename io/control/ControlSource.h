//
// Created by murray on 20/8/25.
//

#pragma once

#include "ControlBase.h"
#include <settings/PttSource.h>
#include <settings/RadioSettingsSource.h>

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
  void notifySettings(const RadioSettings& radioSettings) override
  {
    if (m_pSettingsSink) {
      m_pSettingsSink->applySettings(radioSettings);
    }
  }

  void notifySingleSetting(const SingleSetting& delta) override
  {
    if (m_pSettingsSink) {
      m_pSettingsSink->applySingleSetting(delta);
    }
  }

protected:
  RadioSettingsSink* m_pSettingsSink;
};
