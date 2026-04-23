//
// Created by murray on 2025-08-24.
//

#pragma once

#include <cstdint>
#include <vector>
#include "core/config-settings/config/control/DigitalInputConfig.h"
#include "core/config-settings/settings/base/SettingUpdatePath.h"
#include "DigitalInputLinesRequest.h"
#include "core/config-settings/settings/base/SettingUpdateSource.h"

#ifdef USE_ETL_COLLECTIONS
#include "etl/string.h"
using IdString =etl::string<MAX_ID_LENGTH>;
#else
#include <string>

using IdString = std::string;
#endif

#ifdef MAX_DIGITAL_INPUTS
#undef MAX_DIGITAL_INPUTS
#endif
#define MAX_DIGITAL_INPUTS 16


class DigitalInput : public GpioLines, public SettingUpdateSource
{
public:

  explicit DigitalInput();
  ~DigitalInput() override = default;

  virtual ResultCode configure(const Config::DigitalInput::Fields& config);
  // void setId(const std::string& id) { m_id = id; }
  [[nodiscard]] const IdString& getId() const { return m_id; }
  [[nodiscard]] bool getDebounce() const { return m_debounce; }
  [[nodiscard]] bool getDetectEdge() const { return m_detectEdge; }
  // [[nodiscard]] const GpioLines& getLines() const { return m_lines; }
  [[nodiscard]] const SettingUpdatePath& getSettingPath() const { return m_settingPath; }

  virtual bool handleLineChange(DigitalInputLinesRequest::LineStates& changedLines);

  void connectSettingUpdateSink(SettingUpdateSink* pSink) override;
protected:
  void notifyChange(const DigitalInputLinesRequest::LineState& lineState);
  ResultCode notifySettingUpdate(SettingUpdate& settingDelta) override;

  IdString m_id;
  bool m_activeHigh;
  bool m_debounce;
  bool m_detectEdge;
  SettingUpdatePath m_settingPath;
  SettingUpdateSink* m_pSink;
};
