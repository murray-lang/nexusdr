//
// Created by murray on 2025-08-24.
//

#pragma once

#include <io/control/ControlSource.h>
#include "GpioRotaryEncoder.h"
#include "../Gpio.h"
#include "DigitalInputsRequest.h"
#include <QThread>
#include <config/DigitalInputConfig.h>
#include "config/DigitalInputGroupConfig.h"
#include "io/control/ControlException.h"

class DigitalInputGroup : public ControlSource, public DigitalInputsRequest::Callback
{
public:
  explicit DigitalInputGroup(const char* consumer = "");
  ~DigitalInputGroup() override;

  // ControlBase overrides;
  void configure(const ConfigBase* pConfig) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;



  // GpioLines::Callback override
  void callback(DigitalInputsRequest::LineStates& lineStates) override;

protected:
  void notifySettings(const RadioSettings& radioSettings) override
  {
    throw ControlException("A DigitalInputGroup should not provide RadioSettings, only individual settings.");
  }

  void createInputs(const DigitalInputGroupConfig* pConfig);
  void deleteInputs();

  void createLineToInputMap();
  void readInitialInputStates();

  class InternalSink : public RadioSettingsSink
  {
  public:
    explicit InternalSink(DigitalInputGroup* pGroup) : m_pGroup(pGroup) {}
    void applySettings(const RadioSettings& settings) override
    {
      throw ControlException("A DigitalInput should not provide RadioSettings, only a single setting.");
    }
    void applySingleSetting(const SingleSetting& settingDelta) override
    {
      if (m_pGroup) {
        m_pGroup->notifySingleSetting(settingDelta);
      }
    }

  protected:
    DigitalInputGroup* m_pGroup;
  };

protected:
  InternalSink m_internalSink;
  std::vector<DigitalInput*> m_inputs;
  std::unique_ptr<DigitalInputsRequest> m_pLines;
  std::unordered_map<uint32_t, DigitalInput*> m_lineToInputMap;
};
