//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_RADIOCONTROL_H
#define CUTESDR_VK6HL_RADIOCONTROL_H
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
  void applySingleSetting(const SingleSetting& settingDelta) override
  {
    throw ControlException("RadioControl::applySingleSetting() not implemented");
  }

  // SettingDeltaSink method.

  // PttSink Method
  void ptt(bool on) override;

  // RadioSettingsSource methods
  void connect(RadioSettingsSink* pSink) override;
  void notifySettings(const RadioSettings& radioSettings) override;
  void notifySingleSetting(const SingleSetting& settingDelta) override;

protected:

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


#endif //CUTESDR_VK6HL_RADIOCONTROL_H