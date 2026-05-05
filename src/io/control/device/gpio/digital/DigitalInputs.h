#pragma once

#include <io/control/ControlSource.h>
#include "DigitalInputLinesRequest.h"
#include "core/config-settings/config/control/DigitalInputsConfig.h"
#include "DigitalInputTypes.h"


class DigitalInputs : public ControlSource, public DigitalInputLinesRequest::Callback
{
public:

  explicit DigitalInputs(const char* consumer = "");
  ~DigitalInputs() override = default;

  DigitalInputs(DigitalInputs&& rhs)  noexcept;
  DigitalInputs& operator=(DigitalInputs&& rhs)  noexcept;

  // ControlBase overrides;
  ResultCode configure(const Config::DigitalInputs::Fields& config);
  bool discover() override;
  ResultCode open() override;
  void close() override;
  void exit() override;

  // GpioLines::Callback override
  void callback(DigitalInputLinesRequest::LineStateVector& lineStates) override;

protected:
  ResultCode notifySettings(const RadioSettings& radioSettings) override
  {
    return ResultCode::ERR_NOTIFY_SETTINGS_NOT_IMPLEMENTED;
  }

  ResultCode createInputs(const Config::DigitalInputs::Fields& config);

  void createLineToInputMap();

  void readInitialInputStates();

  class InternalSink : public SettingUpdateSink
  {
  public:
    explicit InternalSink(DigitalInputs& group) : m_group(group) {}

    ResultCode applySettingUpdate(SettingUpdate& settingDelta) override
    {
      return m_group.get().notifySettingUpdate(settingDelta);
    }

  protected:
    reference_wrapper<DigitalInputs> m_group;
  };

protected:
  InternalSink m_internalSink;
  DigitalInputVariantVector m_inputs;
  optional<DigitalInputLinesRequest> m_linesRequest;
  DigitalInputMap m_lineToInputMap;
};
