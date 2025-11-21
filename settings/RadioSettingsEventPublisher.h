//
// Created by murray on 25/9/25.
//

#pragma once

#include <qcoreapplication.h>
#include <QObject>

#include "RadioSettingsEvent.h"
#include "RadioSettingsSink.h"
#include "SingleSettingEvent.h"


class RadioSettingsEventPublisher : public RadioSettingsSink
{
public:
  explicit RadioSettingsEventPublisher(QObject* pTarget) : m_eventTarget(pTarget) {}

  void applySettings(const RadioSettings& settings) override
  {
    // Q_EMIT m_eventTarget->customEvent(new RadioSettingsEvent(settings));
    QCoreApplication::postEvent(m_eventTarget, new RadioSettingsEvent(settings));
  }
  void applySingleSetting(const SingleSetting& setting) override
  {
    QCoreApplication::postEvent(m_eventTarget, new SingleSettingEvent(setting));
  }

protected:
  QObject* m_eventTarget;
};
