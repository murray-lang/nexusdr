#include "QtControlSink.h"

#include <qcoreapplication.h>

#include "QtControlGlobalEventTargets.h"

#include <qeventloop.h>

#include "core/config-settings/settings/events/RadioSettingsEvent.h"
#include "core/config-settings/settings/events/SettingUpdateEvent.h"


QtControlSink::QtControlSink()
  : m_updateSequenceNo(0)
{
}

QtControlSink::QtControlSink(QtControlSink&& rhs) noexcept
  : m_updateSequenceNo(0)
{}

QtControlSink&
QtControlSink::operator=(QtControlSink&& rhs) noexcept
{
  return *this;
}

ResultCode
QtControlSink::configure(const Config::QtControlSink::Fields& config)
{
  return ResultCode::OK;
}

bool
QtControlSink::discover()
{
  return true;
}

ResultCode
QtControlSink::open()
{
  return ResultCode::OK;
}

void
QtControlSink::close()
{
}

void
QtControlSink::exit()
{

}

ResultCode
QtControlSink::applySettings(const RadioSettings& settings)
{
  if (globalControlClientEventTarget != nullptr) {
    auto* rse = new RadioSettingsEvent(settings, ++m_updateSequenceNo, SettingEventBase::BACK_END);
    QCoreApplication::postEvent(globalControlClientEventTarget, rse);
  }
  return ResultCode::OK;
}

ResultCode
QtControlSink::applySettingUpdate(SettingUpdate& settingDelta)
{
  if (globalControlClientEventTarget != nullptr) {
    auto* sue = new SettingUpdateEvent(settingDelta, SettingEventBase::BACK_END);
    QCoreApplication::postEvent(globalControlClientEventTarget, sue);
  }
  return ResultCode::OK;
}

