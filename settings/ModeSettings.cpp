//
// Created by murray on 6/10/25.
//

#include "ModeSettings.h"

#include <vector>

static std::vector<Mode> modes{
    {Mode::AMN, "AM",  "AM (Narrow)",   -3000,  3000,  0},
    {Mode::AMW, "AMW", "AM (Wide)",     -9000,  9000,  0},
    {Mode::LSB, "LSB", "Lower Sideband", -3000,  -300, 0},
    {Mode::USB, "USB", "Upper Sideband",  300,   3000, 0},
    {Mode::FMN, "FM",  "FM (Narrow)",    -5000,  5000, 0},
    {Mode::FMW, "FMW", "FM (Wide)",      -15000, 15000, 0}

};

ModeSettings::ModeSettings() :

 m_cycle(false),
 m_currentIndex(0),
 m_modes(modes)
{

}

bool
ModeSettings::applySetting(const SingleSetting& setting, int startIndex)
{
  bool settingChange = false;
  if (setting.getMeaning() == SingleSetting::DELTA) {
    if (setting.isInt()) {
      m_currentIndex += std::get<int32_t>(setting.getValue());
      settingChange = true;
    } else if (setting.isUInt()) {
      m_currentIndex += static_cast<int>(std::get<uint32_t>(setting.getValue()));
      settingChange = true;
    }
  } else if (setting.getMeaning() == SingleSetting::VALUE) {
    if (setting.isUInt()) {
      auto type = static_cast<Mode::Type>(std::get<uint32_t>(setting.getValue()));
      settingChange = setCurrentMode(type);
    } else if (setting.isMode()) {
      auto type = std::get<Mode::Type>(setting.getValue());
      settingChange = setCurrentMode(type);
    }else if (setting.isString()) {
      auto name = std::get<std::string>(setting.getValue());
      settingChange = setCurrentMode(name);
    }
  }
  return settingChange;
}

int
ModeSettings::getIndexByType(const Mode::Type type) const
{
  for (int i = 0; i < m_modes.size(); i++) {
    if (m_modes[i].getType() == type) {
      return i;
    }
  }
  return -1;
}

int
ModeSettings::getIndexByName(const std::string& name) const
{
  for (int i = 0; i < m_modes.size(); i++) {
    if (m_modes[i].getName() == name) {
      return i;
    }
  }
  return -1;
}

const Mode&
ModeSettings::getModeByType(Mode::Type type) const
{
  int index = getIndexByType(type);
  return m_modes.at(index);
}

const Mode&
ModeSettings::getModeByName(const std::string& name) const
{
  int index = getIndexByName(name);
  return m_modes.at(index);
}

int
ModeSettings::increment()
{
  if (m_cycle) {
    m_currentIndex = (m_currentIndex + 1) % getNumModes();
  } else {
    m_currentIndex = std::min(static_cast<int>(m_modes.size()) - 1, m_currentIndex + 1);
  }
  return m_currentIndex;
}
int
ModeSettings::decrement()
{
  if (m_cycle) {
    int numModes = getNumModes();
    m_currentIndex = (m_currentIndex - 1 + numModes) % numModes;
  } else {
    m_currentIndex = m_currentIndex > 0 ?  m_currentIndex - 1 : 0;
  }
  return m_currentIndex;
}

bool
ModeSettings::setCurrentMode(Mode::Type type)
{
  int index = getIndexByType(type);
  if (index!= m_currentIndex) {
    m_currentIndex = index;
    return true;
  }
  return false;
}

bool
ModeSettings::setCurrentMode(const std::string& name)
{
  int index = getIndexByName(name);
  if (index!= m_currentIndex) {
    m_currentIndex = index;
    return true;
  }
  return false;
}