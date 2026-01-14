//
// Created by murray on 6/10/25.
//

#include "ModeSettings.h"

#include <vector>

static std::vector<Mode> modes{
  {Mode::AMN, "AM",  "AM (Narrow)",    -3000,  3000,    0},
  {Mode::AMW, "AMW", "AM (Wide)",      -9000,  9000,    0},
  {Mode::LSB, "LSB", "Lower Sideband", -4000,  -200,    0},
  {Mode::USB, "USB", "Upper Sideband",   200,   4000,    0},
  {Mode::FMN, "FM",  "FM (Narrow)",    -5000,  5000,    0},
  {Mode::FMW, "FMW", "FM (Wide)",     -15000, 15000,    0},
  {Mode::CWL, "CWL", "CW (Lower)",      -1000,   -50, -700},
  {Mode::CWU, "CWU", "CW (Upper)",        50,   1000,  700}

};

ModeSettings::ModeSettings() :

 m_cycle(false),
 m_currentIndex(0)
 // m_modes(modes)
{

}

const std::vector<Mode>&
ModeSettings::getAll()
{
  return modes;
}

size_t
ModeSettings::getCount()
{
  return modes.size();
}

int
ModeSettings::getNumModes()
{
  return static_cast<int>(modes.size());
}

bool
ModeSettings::applyUpdate(SettingUpdate& setting)
{
  bool settingChange = false;
  if (setting.getMeaning() == SettingUpdate::DELTA) {
    if (setting.isInt()) {
      auto delta = std::get<int32_t>(setting.getValue());
      if (delta > 0) {
        settingChange = increment();
      } else {
        settingChange = decrement();
      }
    } else if (setting.isUInt()) {
      settingChange = increment();
    }
  } else if (setting.getMeaning() == SettingUpdate::VALUE) {
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
ModeSettings::getIndexByType(const Mode::Type type)
{
  for (int i = 0; i < modes.size(); i++) {
    if (modes[i].getType() == type) {
      return i;
    }
  }
  return -1;
}

int
ModeSettings::getIndexByName(const std::string& name)
{
  for (int i = 0; i < modes.size(); i++) {
    if (modes[i].getName() == name) {
      return i;
    }
  }
  return -1;
}

const Mode&
ModeSettings::getModeByType(Mode::Type type)
{
  int index = getIndexByType(type);
  return modes.at(index);
}

const Mode&
ModeSettings::getModeByName(const std::string& name)
{
  int index = getIndexByName(name);
  return modes.at(index);
}

const Mode&
ModeSettings::getModeAt(size_t index)
{
  return modes.at(index);
}

bool
ModeSettings::increment()
{
  if (m_cycle) {
    m_currentIndex = (m_currentIndex + 1) % getNumModes();
    return true;
  } else {
    if (m_currentIndex >= static_cast<int>(modes.size()) - 1) {
      return false;
    }
    m_currentIndex++;
    return true;
  }
}
bool
ModeSettings::decrement()
{
  if (m_cycle) {
    int numModes = getNumModes();
    m_currentIndex = (m_currentIndex - 1 + numModes) % numModes;
    return true;
  } else {
    if (m_currentIndex == 0) {
      return false;
    }
    m_currentIndex--;
    return true;
  }
}

const Mode&
ModeSettings::getCurrentMode() const
{
  return modes.at(m_currentIndex);
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