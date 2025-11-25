//
// Created by murray on 2025-08-24.
//

#pragma once

#include <vector>

#include "ControlSink.h"
#include "ControlSource.h"
#include <config/ControlConfig.h>

#include "ControlException.h"


class RadioControl :
  public RadioSettingsSource,
  public RadioSettingsSink,
  public PttSink
{
public:
  RadioControl();
  ~RadioControl() override = default;

  void configure(const ControlConfig* pConfig);
  void start();
  void stop();

  //RadioSettingsSink methods. Intended for external use, not to be called by internal sources.
  void applySettings(const RadioSettings& settings) override;
  void applySingleSetting(const SingleSetting& setting) override;

  // SettingDeltaSink method.

  // PttSink Method
  void ptt(bool on) override;

  // RadioSettingsSource methods
  void connect(RadioSettingsSink* pSink) override;
  void notifySettings(const RadioSettings& radioSettings) override;
  void notifySingleSetting(const SingleSetting& settingDelta) override;

protected:

  // Intercepts settings from m_controlSources for anything relevant to this RadioControl mechanism
  class InternalSink : public RadioSettingsSink
  {
  public:
    explicit InternalSink(RadioControl* pControl) : m_pControl(pControl) {}
    void applySettings(const RadioSettings& settings) override
    {
      if (m_pControl) {
        m_pControl->notifySettings(settings); // Notify external sink
      }
    }
    void applySingleSetting(const SingleSetting& settingDelta) override
    {
      if (m_pControl) {
        m_pControl->notifySingleSetting(settingDelta); // Notify external sink
      }
    }

  protected:
    RadioControl* m_pControl;
  };
  InternalSink m_internalSink;
  RadioSettingsSink* m_pExternalSettingsSink;
  std::vector<ControlSink*> m_controlSinks;
  std::vector<ControlSource*> m_controlSources;
};
