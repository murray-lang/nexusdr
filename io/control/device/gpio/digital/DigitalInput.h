//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUT_H
#define CUTESDR_VK6HL_DIGITALINPUT_H
#include <cstdint>
#include <vector>
#include <config/DigitalInputConfig.h>
#include "settings/SettingPath.h"
#include "../GpioLines.h"
#include "settings/RadioSettingsSource.h"


class DigitalInput : public RadioSettingsSource
{
public:
  explicit DigitalInput();
  ~DigitalInput() override = default;

  virtual void configure(const DigitalInputConfig* pConfig);
  // void setId(const std::string& id) { m_id = id; }
  [[nodiscard]] const std::string& getId() const { return m_id; }
  [[nodiscard]] const std::vector<uint32_t>& getLines() const { return m_lines; }
  [[nodiscard]] const SettingPath& getSettingPath() const { return m_settingPath; }

  virtual bool handleLineChange(GpioLines::LineStateMap& changedLines) = 0;

  void connect(RadioSettingsSink* pSink) override;
protected:
  void notifySingleSetting(const SingleSetting& settingDelta) override;

  std::string m_id;
  std::vector<uint32_t> m_lines;
  SettingPath m_settingPath;
  RadioSettingsSink* m_pSink;
};


#endif //CUTESDR_VK6HL_DIGITALINPUT_H