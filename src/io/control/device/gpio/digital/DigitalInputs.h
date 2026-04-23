//
// Created by murray on 2025-08-24.
//

#pragma once

#include <io/control/ControlSource.h>
#include "GpioRotaryEncoder.h"
#include "../Gpio.h"
#include "DigitalInputLinesRequest.h"
#include <QThread>
#include "core/config-settings/config/control/DigitalInputsConfig.h"

class DigitalInputs : public ControlSource, public DigitalInputLinesRequest::Callback
{
public:
  explicit DigitalInputs(const char* consumer = "");
  ~DigitalInputs() override;

  DigitalInputs(DigitalInputs&&) = default;
  DigitalInputs& operator=(DigitalInputs&&) = default;

  // ControlBase overrides;
  ResultCode configure(const Config::DigitalInputs::Fields& config);
  bool discover() override;
  ResultCode open() override;
  void close() override;
  void exit() override;

  // GpioLines::Callback override
  void callback(DigitalInputLinesRequest::LineStates& lineStates) override;

protected:
  ResultCode notifySettings(const RadioSettings& radioSettings) override
  {
    return ResultCode::ERR_NOTIFY_SETTINGS_NOT_IMPLEMENTED;
  }

  ResultCode createInputs(const Config::DigitalInputs::Fields& config);
  void deleteInputs();

  void createLineToInputMap();

  void readInitialInputStates();

  class InternalSink : public SettingUpdateSink
  {
  public:
    explicit InternalSink(DigitalInputs* pGroup) : m_pGroup(pGroup) {}
    // void applySettings(const RadioSettings& settings) override
    // {
    //   throw ControlException("A DigitalInput should not provide RadioSettings, only a single setting.");
    // }
    void applySettingUpdate(SettingUpdate& settingDelta) override
    {
      if (m_pGroup) {
        m_pGroup->notifySettingUpdate(settingDelta);
      }
    }

  protected:
    DigitalInputs* m_pGroup;
  };

protected:
  InternalSink m_internalSink;
  std::vector<DigitalInput*> m_inputs;
  std::unique_ptr<DigitalInputLinesRequest> m_pLines;
  std::unordered_map<uint32_t, DigitalInput*> m_lineToInputMap;
};
