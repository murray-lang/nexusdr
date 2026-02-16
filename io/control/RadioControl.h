//
// Created by murray on 2025-08-24.
//

#pragma once

#include <vector>

#include "ControlSink.h"
#include "ControlSource.h"
#include "config-settings/config/ControlConfig.h"

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
  // void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  void applySettingUpdate(SettingUpdate& setting) override;

  // SettingDeltaSink method.

  // PttSink Method
  void ptt(bool on) override;

  // RadioSettingsSource methods
  void connect(RadioSettingsSink* pSink) override;
  void notifySettings(const RadioSettings& radioSettings) override;
  void notifySettingUpdate(SettingUpdate& settingDelta) override;

protected:

  // Intercepts settings from m_controlSources for anything relevant to this RadioControl mechanism
  // One of these gets connect()ed to each control source
  // ControlSources have no concept of BandSettings, since these are internal to the radio.
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
    void applySettingUpdate(SettingUpdate& settingDelta) override
    {
      if (m_pControl) {
        m_pControl->notifySettingUpdate(settingDelta); // Notify external sink
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
