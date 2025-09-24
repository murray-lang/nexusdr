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
Radio::configure(const RadioConfig* pConfig)
{
  m_control.configure(pConfig->getControl());
  m_pReceiver->configure(pConfig->getReceiver());

}

void
Radio::start()
{
  m_pReceiver->start();
  m_control.start();
}

void
Radio::stop()
{
  m_control.stop();
  m_pReceiver->stop();
}

void
Radio::applySettings(const RadioSettings& settings)
{
  m_control.applySettings(settings);
  m_pReceiver->apply(settings.rxSettings);
}

void
Radio::applySingleSetting(const SettingDelta& settingDelta)
{

}

void Radio::ptt(bool on)
{
  if (on) {
    pttOn();
  } else {
    pttOff();
  }
}

void
Radio::pttOn()
{
  if (m_pReceiver != nullptr) {
    m_pReceiver->ptt(true);
  }
  m_control.ptt(true);
}

void
Radio::pttOff()
{
  m_control.ptt(false);
  if (m_pReceiver != nullptr) {
    m_pReceiver->ptt(false);
  }
}

