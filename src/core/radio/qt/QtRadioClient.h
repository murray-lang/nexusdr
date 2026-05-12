#pragma once
#include "core/radio/RadioBase.h"

class RadioSettingsEvent;
class SettingUpdateEvent;

class QtRadioClient : public QObject, public RadioBase
{
  Q_OBJECT
public:
  QtRadioClient(QObject* parent);

  ResultCode configure(const Config::Radio::Fields& config) override;
  ResultCode start(EventTarget* pEventTarget) override;
  void stop() override;

  ResultCode applySettings(const RadioSettings& settings) override
  {
    return ResultCode::OK;
  }

  ResultCode applySettingUpdate(SettingUpdate& update) override;

  void ptt(bool on) override {}

  void customEvent(QEvent* event) override;

signals:
  void radioSettingsReceived(const RadioSettings& settings, uint64_t sequence);
  void settingUpdateReceived(const SettingUpdate& update);

protected:
  ResultCode requestCurrentSettings();

private:
  QObject* m_pParent;
};