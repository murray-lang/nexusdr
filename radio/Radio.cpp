//
// Created by murray on 18/08/25.
//

#include "Radio.h"


#define SAMPLE_RATE 192000

Radio::Radio(QObject *eventTarget)
{
  m_pReceiver = new IqReceiver(eventTarget);
}

Radio::~Radio()
{
  delete m_pReceiver;
}

void
Radio::configure(const RadioConfig& config)
{
  m_pReceiver->configure(config.getReceiver());
}

void
Radio::start() const
{
  m_pReceiver->start();
}

void
Radio::stop() const
{
  m_pReceiver->stop();
}

void
Radio::apply(const RadioSettings& settings)
{
  m_pReceiver->apply(settings.rxSettings);
}
