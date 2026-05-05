//
// Created by murray on 7/1/26.
//

#pragma once
#include "ResultCode.h"
#include "SettingUpdate.h"

class SettingUpdateSink
{
public:
  virtual ~SettingUpdateSink() = default;
  virtual ResultCode applySettingUpdate(SettingUpdate& settingDelta) = 0;

  virtual ResultCode applySettingUpdates(SettingUpdate* updates, std::size_t count)
  {
    ResultCode rc = ResultCode::OK;
    if (!updates) return rc;
    for (std::size_t i = 0; i < count; ++i) {
      updates[i].resetCursor();
      rc = applySettingUpdate(updates[i]);
      if (rc != ResultCode::OK) {
        break;
      }
    }
    return rc;
  }

  // Convenience for std::array + prefix length (optional)
  template <std::size_t N>
  ResultCode applySettingUpdates(std::array<SettingUpdate, N>& updates, std::size_t count)
  {
    if (count > N) count = N;
    return applySettingUpdates(updates.data(), count);
  }
};
