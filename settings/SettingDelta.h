//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_SETTINGDELTA_H
#define CUTESDR_VK6HL_SETTINGDELTA_H
#include "SettingPath.h"


class SettingDelta
{
public:
  SettingDelta();
  ~SettingDelta();

protected:
  SettingPath m_settingPath;
  int32_t m_delta;
};

#endif //CUTESDR_VK6HL_SETTINGDELTA_H