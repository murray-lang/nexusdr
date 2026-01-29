//
// Created by murray on 5/10/25.
//

#pragma once

#include <unordered_map>
#include <vector>

#include "Mode.h"
#include "base/SettingsBase.h"

// using Modes = std::unordered_map<Mode::Type, Mode>;

class ModeSettings : SettingsBase
{
public:
  ModeSettings();
  ModeSettings(const ModeSettings& rhs) = default;
  ~ModeSettings() override = default;
  ModeSettings& operator=(const ModeSettings& rhs) = default;

  bool applyUpdate(SettingUpdate& setting) override;
  void clearChanged() override
  {
    SettingsBase::clearChanged();
  }
  void setCycle(bool cycle) { m_cycle = cycle;}

  static const std::vector<Mode>& getAll();
  [[nodiscard]] bool getCycle() const { return m_cycle; }
  static size_t getCount();
  static int getIndexByType(Mode::Type type);
  static int getIndexByName(const std::string& name);
  static const Mode& getModeByType(Mode::Type type);
  static const Mode& getModeByName(const std::string& name);
  static const Mode& getModeAt(size_t index);
  [[nodiscard]] const Mode& getCurrentMode() const;
  [[nodiscard]] int getCurrentIndex() const { return m_currentIndex; }
  static int getNumModes();
  bool increment();
  bool decrement();

  bool setCurrentMode(Mode::Type type);
  bool setCurrentMode(const std::string& name);

protected:
  bool m_cycle;
  int m_currentIndex;
  // std::vector<Mode> m_modes;
};
