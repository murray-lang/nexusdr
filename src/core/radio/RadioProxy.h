//
// Created by murray on 23/3/26.
//

#pragma once
#include "RadioBase.h"
#include "core/config-settings/settings/bands/BandSelector.h"


class RadioProxy : public RadioBase
{
public:
  explicit RadioProxy(EventTarget *pEventTarget = nullptr);
  ~RadioProxy() override = default;

  void configure(const RadioConfig* pConfig) override;
  void start() override;
  void stop() override;

  ResultCode applySettings(const RadioSettings& settings) override;

  ResultCode applySettingUpdate(SettingUpdate& update) override;

protected:
  void notifyUpdate(const SettingUpdate& update);


};
