//
// Created by murray on 2025-08-24.
//

#pragma once

#include <io/control/ControlSource.h>
#include "GpioRotaryEncoder.h"
#include "../Gpio.h"
#include "DigitalInputLinesRequest.h"
#include <QThread>
#include "../../../../../core/config-settings/config/control/DigitalInputsConfig.h"
#include "io/control/ControlException.h"

class DigitalInputs : public ControlSource, public DigitalInputLinesRequest::Callback
{
public:
  explicit DigitalInputs(const char* consumer = "");
  ~DigitalInputs() override;

  // ControlBase overrides;
  void configure(const ConfigBase* pConfig) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;

  // GpioLines::Callback override
  void callback(DigitalInputLinesRequest::LineStates& lineStates) override;

protected:
  void notifySettings(const RadioSettings& radioSettings) override
  {
    throw ControlException("A DigitalInputs should not provide RadioSettings, only individual settings.");
  }

  void createInputs(const DigitalInputsConfig* pConfig);
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
