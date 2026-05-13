#include "QtRadioClient.h"

#include <qcoreapplication.h>

#include "QtGlobalEventTargets.h"
#include "core/config-settings/settings/events/RadioSettingsEvent.h"
#include "core/config-settings/settings/events/SettingUpdateEvent.h"
#include "core/radio/receiver/events/ReceiverAudioEvent.h"
#include "core/radio/receiver/events/ReceiverIqEvent.h"
#include "core/radio/receiver/events/ReceiverMeterEvent.h"
#include "core/radio/transmitter/events/TransmitterAudioEvent.h"
#include "core/radio/transmitter/events/TransmitterIqEvent.h"

QtRadioClient::QtRadioClient(QObject* parent)
  : m_pParent(parent)
{

}

ResultCode
QtRadioClient::configure(const Config::Radio::Fields& config)
{
  return ResultCode::OK;
}

ResultCode
QtRadioClient::start()
{
  globalControlClientEventTarget = this;
  globalMeteringClientEventTarget = this;
  globalMonitorClientEventTarget = this;
  return requestCurrentSettings();
}

void
QtRadioClient::stop()
{
  globalControlClientEventTarget = nullptr;
  globalMeteringClientEventTarget = nullptr;
  globalMonitorClientEventTarget = nullptr;
}

void
QtRadioClient::customEvent(QEvent* event)
{
  if (event->type() == RadioSettingsEvent::RadioSettingsEventType) {

    const auto* settingsEvent = dynamic_cast<RadioSettingsEvent*>(event);
    emit radioSettingsReceived(settingsEvent->getRadioSettings(), settingsEvent->getSequence());

  } else if (event->type() == SettingUpdateEvent::SettingUpdateEventType) {

    const auto* updateEvent = dynamic_cast<SettingUpdateEvent*>(event);
    emit settingUpdateReceived(updateEvent->m_setting);

  } else if (event->type() == ReceiverIqEvent::RxIqEvent) {

    const auto* iqEvent = dynamic_cast<ReceiverIqEvent*>(event);
    emit receiverIqReceived(iqEvent->buffer.get(), iqEvent->dataLength, iqEvent->sampleRate);

  } else if (event->type() == ReceiverAudioEvent::RxAudioEvent) {

    const auto* audioEvent = dynamic_cast<ReceiverAudioEvent*>(event);
    emit receiverAudioReceived(audioEvent->buffer.get(), audioEvent->dataLength, audioEvent->sampleRate);

  } else if (event->type() == ReceiverMeterEvent::RxMeterEvent) {

    const auto* meterEvent = dynamic_cast<ReceiverMeterEvent*>(event);
    emit meteringReceived(meterEvent->metering());

  } else if (event->type() == TransmitterIqEvent::TxIqEvent) {

    const auto* iqEvent = dynamic_cast<TransmitterIqEvent*>(event);
    emit transmitterIqReceived(iqEvent->buffer.get(), iqEvent->dataLength, iqEvent->sampleRate);

  } else if (event->type() == TransmitterAudioEvent::TxAudioEvent) {

    const auto* audioEvent = dynamic_cast<TransmitterAudioEvent*>(event);
    emit transmitterAudioReceived(audioEvent->buffer.get(), audioEvent->dataLength, audioEvent->sampleRate);
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