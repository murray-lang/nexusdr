#pragma once
#include <QObject>

#include "core/config-settings/config/control/QtControlSinkConfig.h"
#include "io/control/ControlSink.h"

class QtControlSink : public QObject, public ControlSink
{
  Q_OBJECT
public:
  QtControlSink();
  QtControlSink(QtControlSink&& rhs) noexcept;
  ~QtControlSink() override = default;

  QtControlSink& operator=(QtControlSink&& rhs) noexcept;

  ResultCode configure(const Config::QtControlSink::Fields& config);

  bool discover() override;
  ResultCode open() override;
  void close() override;
  void exit() override;

  ResultCode applySettings(const RadioSettings& settings) override;
  ResultCode applySettingUpdate(SettingUpdate& settingDelta) override;

  void ptt(bool on) override {}

protected:
  uint64_t m_updateSequenceNo;

};