//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUT_H
#define CUTESDR_VK6HL_DIGITALINPUT_H
#include <cstdint>
#include <vector>
#include <config/DigitalInputConfig.h>
#include "settings/SettingPath.h"
#include "DigitalInputsRequest.h"
#include "settings/RadioSettingsSource.h"


class DigitalInput : public GpioLines, public RadioSettingsSource
{
public:

  explicit DigitalInput();
  ~DigitalInput() override = default;

  virtual void configure(const DigitalInputConfig* pConfig);
  // void setId(const std::string& id) { m_id = id; }
  [[nodiscard]] const std::string& getId() const { return m_id; }
  [[nodiscard]] bool getDebounce() const { return m_debounce; }
  // [[nodiscard]] const GpioLines& getLines() const { return m_lines; }
  [[nodiscard]] const SettingPath& getSettingPath() const { return m_settingPath; }

  virtual bool handleLineChange(DigitalInputsRequest::LineStateMap& changedLines) = 0;

  void connect(RadioSettingsSink* pSink) override;
protected:
  void notifySingleSetting(const SingleSetting& settingDelta) override;

  std::string m_id;
  bool m_debounce;
  SettingPath m_settingPath;
  RadioSettingsSink* m_pSink;
};


#endif //CUTESDR_VK6HL_DIGITALINPUT_H