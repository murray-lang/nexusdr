//
// Created by murray on 7/1/26.
//

#pragma once
#include "SingleSetting.h"

class SingleSettingSink
{
public:
  virtual ~SingleSettingSink() = default;
  virtual void applySingleSetting(const SingleSetting& settingDelta) = 0;
};
