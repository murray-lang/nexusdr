//
// Created by murray on 5/10/25.
//

#ifndef CUTESDR_VK6HL_MODES_H
#define CUTESDR_VK6HL_MODES_H
#include <unordered_map>
#include <vector>

#include "Mode.h"
#include "SettingsBase.h"

// using Modes = std::unordered_map<Mode::Type, Mode>;

class ModeSettings : SettingsBase
{
public:
  ModeSettings();
  ModeSettings(const ModeSettings& rhs) = default;
  ~ModeSettings() override = default;
  ModeSettings& operator=(const ModeSettings& rhs) = default;

  bool applySetting(const SingleSetting& setting, int startIndex) override;
  void clearChanged() override
  {
    SettingsBase::clearChanged();
  }
  void setCycle(bool cycle) { m_cycle = cycle;}

  [[nodiscard]] const std::vector<Mode>& getAll() const { return m_modes; }
  [[nodiscard]] bool getCycle() const { return m_cycle; }
  [[nodiscard]] size_t getCount() const { return m_modes.size(); }
  [[nodiscard]] int getIndexByType(Mode::Type type) const;
  [[nodiscard]] int getIndexByName(const std::string& name) const;
  [[nodiscard]] const Mode& getModeByType(Mode::Type type) const;
  [[nodiscard]] const Mode& getModeByName(const std::string& name) const;
  [[nodiscard]] const Mode& getModeAt(size_t index) { return m_modes.at(index); }
  [[nodiscard]] const Mode& getCurrentMode() { return m_modes.at(m_currentIndex); }
  [[nodiscard]] int getCurrentIndex() const { return m_currentIndex; }
  [[nodiscard]] int getNumModes() const { return static_cast<int>(m_modes.size()); }
  bool increment();
  bool decrement();

  bool setCurrentMode(Mode::Type type);
  bool setCurrentMode(const std::string& name);

protected:
  bool m_cycle;
  int m_currentIndex;
  std::vector<Mode> m_modes;
};

#endif //CUTESDR_VK6HL_MODES_H