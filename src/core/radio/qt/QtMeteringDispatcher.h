#pragma once
#include <QObject>

#include "core/radio/MeteringSink.h"

class QtMeteringDispatcher: public MeteringSink
{
public:
  QtMeteringDispatcher(function<QObject*()> eventTargetProvider);
  ~QtMeteringDispatcher() override = default;

  void setEventTarget(QObject* eventTarget);

  void updateRxMetering(const IqReceiverMetering& metering) override;
private:
  function<QObject*()> m_eventTargetProvider;
};
