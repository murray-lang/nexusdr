#pragma once
#include "SettingsCrossPlatformTypes.h"

#include "Mode.h"
#include "base/SettingsBase.h"

#ifdef USE_ETL
using ModeVector = etl::vector<Mode, MAX_MODES>;
#else
using ModeVector = std::vector<Mode>;
#endif

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

  static const ModeVector& getAll();
  [[nodiscard]] bool getCycle() const { return m_cycle; }
  static size_t getCount();
  static int getIndexByType(Mode::Type type);
  static int getIndexByName(const ModeNameString& name);
  static const Mode& getModeByType(Mode::Type type);
  static const Mode& getModeByName(const ModeNameString& name);
  static const Mode& getModeAt(size_t index);
  [[nodiscard]] const Mode& getCurrentMode() const;
  [[nodiscard]] int getCurrentIndex() const { return m_currentIndex; }
  static int getNumModes();
  bool increment();
  bool decrement();

  bool setCurrentMode(Mode::Type type);
  bool setCurrentMode(const ModeNameString& name);

protected:
  bool m_cycle;
  int m_currentIndex;
  // std::vector<Mode> m_modes;
};
