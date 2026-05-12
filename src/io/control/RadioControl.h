#pragma once



#include "ControlSink.h"
#include "ControlSource.h"
#include "ControlSinkFactory.h"
#include "ControlSourceFactory.h"
#include "core/config-settings/config/control/ControlConfig.h"
#include "ResultCode.h"
#include "etl/vector.h"

#ifdef USE_ETL
#include <etl/vector.h>

using ControlSinkVector = etl::vector<ControlSinkVariant, MAX_CONTROL_SINKS>; //
using ControlSourceVector = etl::vector<ControlSourceVariant, MAX_CONTROL_SOURCES>;
#else
#include <vector>

using ControlSinkVector = std::vector<ControlSinkVariant>;
using ControlSourceVector = std::vector<ControlSourceVariant>;
#endif

class RadioControl :
  public RadioSettingsSource,
  public RadioSettingsSink,
  public PttSink
{
public:
  RadioControl();
  ~RadioControl() override = default;

  ResultCode configure(const Config::Control::Fields& pConfig);
  ResultCode start();
  void stop();

  //RadioSettingsSink methods. Intended for external use, not to be called by internal sources.
  ResultCode applySettings(const RadioSettings& settings) override;
  // void applySettings(const RadioSettings& settings, BandSettings* pBandSettings) override;
  ResultCode applySettingUpdate(SettingUpdate& setting) override;

  // SettingDeltaSink method.

  // PttSink Method
  void ptt(bool on) override;

  // RadioSettingsSource methods
  void connectSink(RadioSettingsSink* pSink) override;
  ResultCode notifySettings(const RadioSettings& radioSettings) override;
  ResultCode notifySettingUpdate(SettingUpdate& settingDelta) override;

protected:

  // Intercepts settings from m_controlSources for anything relevant to this RadioControl mechanism
  // One of these gets connect()ed to each control source
  // ControlSources have no concept of BandSettings, since these are internal to the radio.
  class InternalSink : public RadioSettingsSink
  {
  public:
    explicit InternalSink(RadioControl* pControl) : m_pControl(pControl) {}
    ResultCode applySettings(const RadioSettings& settings) override
    {
      if (m_pControl) {
        return m_pControl->notifySettings(settings); // Notify external sink
      }
      return ResultCode::OK;
    }
    ResultCode applySettingUpdate(SettingUpdate& settingDelta) override
    {
      if (m_pControl) {
        return m_pControl->notifySettingUpdate(settingDelta); // Notify external sink
      }
      return ResultCode::OK;
    }

  protected:
    RadioControl* m_pControl;
  };
  InternalSink m_internalSink;
  RadioSettingsSink* m_pExternalSettingsSink;
  ControlSinkVector m_controlSinks;
  ControlSourceVector m_controlSources;
};
