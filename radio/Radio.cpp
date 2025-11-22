//
// Created by murray on 18/08/25.
//

#include "Radio.h"

#include <qcoreapplication.h>

#include "settings/RadioSettings.h"
#include "settings/RadioSettingsEvent.h"
#include "settings/SingleSettingEvent.h"


Radio::Radio(QObject *pEventTarget) :
  m_settings(),
  m_pReceiver(nullptr),
  m_pTransmitter(nullptr),
  m_control(),
  m_pEventTarget(pEventTarget)
{
  m_pReceiver = new IqReceiver(pEventTarget);
  m_pTransmitter = new IqTransmitter(pEventTarget);
}

Radio::~Radio()
{
  delete m_pReceiver;
  delete m_pTransmitter;
}

void
Radio::configure(const RadioConfig* pConfig)
{
  m_control.configure(pConfig->getControl());

  const ReceiverConfig* pRxConfig = pConfig->getReceiver();
  if (pRxConfig != nullptr) {
    m_pReceiver = new IqReceiver(m_pEventTarget);
    m_pReceiver->configure(pRxConfig);
  }

  const TransmitterConfig* pTxConfig = pConfig->getTransmitter();
  if (pTxConfig != nullptr) {
    m_pTransmitter = new IqTransmitter(m_pEventTarget);
    m_pTransmitter->configure(pTxConfig);
  }
}

void
Radio::start()
{
  m_control.connect(this);
  if (m_pReceiver != nullptr) {
    m_pReceiver->start();
  }
  m_control.start();
}

void
Radio::stop()
{
  m_control.stop();
  m_control.connect(nullptr);
  if (m_pReceiver != nullptr) {
    m_pReceiver->stop();
  }
  if (m_pTransmitter != nullptr) {
    m_pTransmitter->stop();
  }
}

void
Radio::applySettings(const RadioSettings& settings)
{
  if (&settings != &m_settings) {
    m_settings = settings;
  }
  if (settings.changed & RadioSettings::PTT) {
    ptt(m_settings.ptt);
    return; // Don't try to do anything else concurrently with PTT.
  }
  if (settings.changed & RadioSettings::MODE) {
    if (m_pReceiver != nullptr) {
      m_pReceiver->setMode(m_settings.mode);
    }
    if (m_pTransmitter != nullptr) {
      m_pTransmitter->setMode(m_settings.mode);
    }
  }
  m_control.applySettings(m_settings);
  if (settings.changed & RadioSettings::RX) {
    if (m_pReceiver != nullptr) {
      m_pReceiver->apply(m_settings.rxSettings);
    }
  }
  if (settings.changed & RadioSettings::TX) {
    if (m_pTransmitter != nullptr) {
      m_pTransmitter->apply(m_settings.txSettings);
    }
  }
  if (m_pEventTarget != nullptr) {
    // qDebug() << "Radio::applySettings posting RadioSettingsEvent";
    // qDebug() << m_settings.mode.getName().c_str();
    QCoreApplication::postEvent(m_pEventTarget, new RadioSettingsEvent(settings));
  }
  m_settings.clearChanged();
}

void
Radio::applySingleSetting(const SingleSetting& setting)
{
  if (m_pEventTarget != nullptr) {
    // QCoreApplication::postEvent(m_pEventTarget, new SingleSettingEvent(setting));
  }
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
  if (m_pTransmitter != nullptr) {
    m_pTransmitter->ptt(true);
  }
}

void
Radio::pttOff()
{
  if (m_pTransmitter != nullptr) {
    m_pTransmitter->ptt(false);
  }
  m_control.ptt(false);
  if (m_pReceiver != nullptr) {
    m_pReceiver->ptt(false);
  }
}

