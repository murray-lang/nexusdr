//
// Created by murray on 2025-08-24.
//

#pragma once

#include <cstdint>
#include <vector>
#include "../../../../../core/config-settings/config/control/DigitalInputConfig.h"
#include "core/config-settings/settings/base/SettingUpdatePath.h"
#include "DigitalInputLinesRequest.h"
#include "core/config-settings/settings/base/SettingUpdateSource.h"
#include "io/control/ControlException.h"

#ifdef MAX_DIGITAL_INPUTS
#undef MAX_DIGITAL_INPUTS
#endif
#define MAX_DIGITAL_INPUTS 16


class DigitalInput : public GpioLines, public SettingUpdateSource
{
public:

  explicit DigitalInput();
  ~DigitalInput() override = default;

  virtual void configure(const DigitalInputConfig* pConfig);
  // void setId(const std::string& id) { m_id = id; }
  [[nodiscard]] const std::string& getId() const { return m_id; }
  [[nodiscard]] bool getDebounce() const { return m_debounce; }
  [[nodiscard]] bool getDetectEdge() const { return m_detectEdge; }
  // [[nodiscard]] const GpioLines& getLines() const { return m_lines; }
  [[nodiscard]] const SettingUpdatePath& getSettingPath() const { return m_settingPath; }

  virtual bool handleLineChange(DigitalInputLinesRequest::LineStates& changedLines);

  void connectSettingUpdateSink(SettingUpdateSink* pSink) override;
protected:
  void notifyChange(const DigitalInputLinesRequest::LineState& lineState);
  void notifySettingUpdate(SettingUpdate& settingDelta) override;

  std::string m_id;
  bool m_activeHigh;
  bool m_debounce;
  bool m_detectEdge;
  SettingUpdatePath m_settingPath;
  SettingUpdateSink* m_pSink;
};
