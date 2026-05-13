#include "QtControlSource.h"
#include "core/radio/qt/QtGlobalEventTargets.h"

#include <qeventloop.h>

#include "core/config-settings/settings/events/RadioSettingsEvent.h"
#include "core/config-settings/settings/events/SettingUpdateEvent.h"


QtControlSource::QtControlSource()
  : m_thread(this)
{
}

QtControlSource::QtControlSource(QtControlSource&& rhs) noexcept
  : ControlSource(move(rhs))
  , m_thread(this)
{
}

QtControlSource::~QtControlSource()
{
  QtControlSource::close();
}

QtControlSource&
QtControlSource::operator=(QtControlSource&& rhs) noexcept
{
  ControlSource::operator=(move(rhs));
  return *this;
}

ResultCode
QtControlSource::configure(const Config::QtControlSource::Fields& config)
{
  return ResultCode::OK;
}

bool
QtControlSource::discover()
{
  return true;
}

ResultCode
QtControlSource::open()
{
  globalControlRadioEventTarget = this;
  m_thread.start();
  return ResultCode::OK;
}

void
QtControlSource::close()
{
  globalControlRadioEventTarget = nullptr;
  // emit finished();
  m_thread.join();
}

void
QtControlSource::exit()
{

}

void
QtControlSource::run()
{
  QEventLoop loop;
  // QObject::connect(this, &QtControlSource::finished, &loop, &QEventLoop::quit);
  loop.exec();
}

void
QtControlSource::customEvent(QEvent* event)
{
  if (event->type() == RadioSettingsEvent::RadioSettingsEventType) {

    auto* radioSettingsEvent = dynamic_cast<RadioSettingsEvent*>(event);
    notifySettings(radioSettingsEvent->getRadioSettings());

  } else if (event->type() == SettingUpdateEvent::SettingUpdateEventType) {

    auto* settingUpdateEvent = dynamic_cast<SettingUpdateEvent*>(event);
    notifySettingUpdate(settingUpdateEvent->m_setting);
  }
}