//
// Created by murray on 18/08/25.
//

#include "Radio.h"

#include "settings/RadioSettings.h"


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
  m_control.connect(this);
  m_pReceiver->start();
  m_control.start();
}

void
Radio::stop()
{
  m_control.stop();
  m_control.connect(nullptr);
  m_pReceiver->stop();
}

void
Radio::applySettings(const RadioSettings& settings)
{
  if (&settings != &m_settings) {
    m_settings = settings;
  }
  m_control.applySettings(m_settings);
  m_pReceiver->apply(m_settings.rxSettings);
  m_settings.clearChanged();
}

void
Radio::applySingleSetting(const SingleSetting& setting)
{
  if (m_settings.applySetting(setting, 0)) {
    applySettings(m_settings);
  }
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

