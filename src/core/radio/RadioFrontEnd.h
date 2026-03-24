//
// Created by murray on 23/3/26.
//

#pragma once
#include "RadioBase.h"
#include "core/config-settings/settings/bands/BandSelector.h"


class RadioFrontEnd : public RadioBase
{
public:
  explicit RadioFrontEnd(EventTarget *pEventTarget = nullptr);
  ~RadioFrontEnd() override = default;

  void configure(const RadioConfig* pConfig) override;
  void start() override;
  void stop() override;

  void applySettings(const RadioSettings& settings) override;

  void applySettingUpdate(SettingUpdate& update) override;

protected:
  void notifyUpdate(const SettingUpdate& update);

private:
  BandSelector m_bandSelector;
};
