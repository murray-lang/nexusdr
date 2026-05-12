#include "QtRadioClient.h"

#include <qcoreapplication.h>

#include "core/config-settings/settings/events/RadioSettingsEvent.h"
#include "core/config-settings/settings/events/SettingUpdateEvent.h"
#include "io/control/qt/QtControlGlobalEventTargets.h"

QtRadioClient::QtRadioClient(QObject* parent)
  : RadioBase(this)
  , m_pParent(parent)
{

}

ResultCode
QtRadioClient::configure(const Config::Radio::Fields& config)
{
  return ResultCode::OK;
}

ResultCode
QtRadioClient::start(EventTarget* pEventTarget)
{
  globalControlClientEventTarget = this;
  return requestCurrentSettings();
}

void
QtRadioClient::stop()
{
  globalControlClientEventTarget = nullptr;
}

void
QtRadioClient::customEvent(QEvent* event)
{
  if (event->type() == RadioSettingsEvent::RadioSettingsEventType) {
    auto* settingsEvent = dynamic_cast<RadioSettingsEvent*>(event);
    if (settingsEvent) {
      emit radioSettingsReceived(settingsEvent->getRadioSettings(), settingsEvent->getSequence());
    }
  } else if (event->type() == SettingUpdateEvent::SettingUpdateEventType) {
    auto* updateEvent = dynamic_cast<SettingUpdateEvent*>(event);
    if (updateEvent) {
      emit settingUpdateReceived(updateEvent->m_setting);
    }
  }
}

ResultCode
QtRadioClient::requestCurrentSettings()
{
  SettingUpdatePath path({RadioSettings::NOTIFY_CONTROL_SINKS});
  SettingUpdate setting(path, true, SettingUpdate::Meaning::VALUE);
  return applySettingUpdate(setting);
}

ResultCode
QtRadioClient::applySettingUpdate(SettingUpdate& update)
{
  if (globalControlRadioEventTarget != nullptr) {
   auto* sue = new SettingUpdateEvent(update, SettingEventBase::FRONT_END);
   QCoreApplication::postEvent(globalControlRadioEventTarget, sue);
   return ResultCode::OK;
  }
  return ResultCode::ERR_CONTROL_NO_EVENT_TARGET;
}