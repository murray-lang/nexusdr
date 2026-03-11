//
// Created by murray on 7/1/26.
//

#pragma once
#include "SettingUpdate.h"

class SettingUpdateSink
{
public:
  virtual ~SettingUpdateSink() = default;
  virtual void applySettingUpdate(SettingUpdate& settingDelta) = 0;

  virtual void applySettingUpdates(SettingUpdate* updates, std::size_t count)
  {
    if (!updates) return;
    for (std::size_t i = 0; i < count; ++i) {
      updates[i].resetCursor();
      applySettingUpdate(updates[i]);
    }
  }

  // Convenience for std::array + prefix length (optional)
  template <std::size_t N>
  void applySettingUpdates(std::array<SettingUpdate, N>& updates, std::size_t count)
  {
    if (count > N) count = N;
    applySettingUpdates(updates.data(), count);
  }
};
