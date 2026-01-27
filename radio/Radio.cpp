//
// Created by murray on 18/08/25.
//

#include "Radio.h"

#include <qcoreapplication.h>

#include "settings/RadioSettings.h"
#include "settings/RadioSettingsEvent.h"
#include "../settings/core/SettingUpdateEvent.h"
#include <QDebug>


Radio::Radio(QObject *pEventTarget) :
  m_settings(),
  m_pReceiver(nullptr),
  m_pTransmitter(nullptr),
  m_pControl(nullptr),
  m_pEventTarget(pEventTarget)
{
  m_pReceiver = new IqReceiver(pEventTarget);
  m_pTransmitter = new IqTransmitter(pEventTarget);
}

Radio::~Radio()
{
  delete m_pReceiver;
  delete m_pTransmitter;
  delete m_pControl;
}

void
Radio::configure(const RadioConfig* pConfig)
{
  const ControlConfig* pControlConfig = pConfig->getControl();
  if (pControlConfig != nullptr) {
    m_pControl = new RadioControl();
    m_pControl->configure(pControlConfig);
  }
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
  if (m_pControl != nullptr) {
    m_pControl->connect(this);
  }
  if (m_pReceiver != nullptr) {
    m_pReceiver->start();
  }
  if (m_pControl != nullptr) {
    m_pControl->start();
  }

}

void
Radio::stop()
{
  if (m_pControl != nullptr) {
    m_pControl->stop();
    m_pControl->connect(nullptr);
  }
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
  if (settings.hasSettingChanged(RadioSettings::PTT)) {
    ptt(m_settings.getPtt());
    m_settings.clearChanged();
    return; // Don't try to do anything else concurrently with PTT.
  }

  BandSettings* pBandSettings = settings.getFocusBandSettings();
  if (settings.hasSettingChanged(RadioSettings::BAND)) {

    RxPipelineSettings* rxPipelineSettings = pBandSettings->getFocusRxPipelineSettings();
    if (m_pReceiver != nullptr) {
      m_pReceiver->adjustRfSettingsToLimits(rxPipelineSettings->getRfSettings());
      m_pReceiver->apply(rxPipelineSettings);
    }
    TxPipelineSettings* txPipelineSettings = pBandSettings->getTxPipelineSettings();
    if (m_pTransmitter != nullptr) {
      m_pTransmitter->adjustRfSettingsToLimits(txPipelineSettings->getRfSettings());
      m_pTransmitter->apply(txPipelineSettings);
    }
  }
  if (m_pControl != nullptr) {
    m_pControl->applySettings(m_settings);
  }

  if (settings.hasSettingChanged(RadioSettings::RX)) {
    if (m_pReceiver != nullptr) {
      m_pReceiver->apply(m_settings.getRxSettings());
    }
  }
  if (settings.hasSettingChanged(RadioSettings::TX)) {
    if (m_pTransmitter != nullptr) {
      m_pTransmitter->apply(m_settings.getTxSettings());
    }
  }
  if (m_pEventTarget != nullptr) {
    // qDebug() << "Radio::applySettings posting RadioSettingsEvent";
    // qDebug() << m_settings.mode.getName().c_str();
    RadioSettingsEvent* rse = new RadioSettingsEvent(settings);
    QCoreApplication::postEvent(m_pEventTarget, rse);
  }
  m_settings.clearChanged();
  pBandSettings->clearChanged();
}

void
Radio::applySettingUpdate(SettingUpdate& update)
{
  if (m_pEventTarget != nullptr) {
    // QCoreApplication::postEvent(m_pEventTarget, new SingleSettingEvent(setting));
  }
  if (m_settings.applyUpdate(update)) {
    applySettings(m_settings);
  }
}

void
Radio::applyBand(const std::string& bandName)
{
  // qDebug() << "Radio::applyBand(): applying band " << bandName.c_str() << ". Existing band: " << m_settings.bandName.c_str() ;
  SettingUpdatePath bandPath({RadioSettings::BAND, BandSelector::SELECT});
  SettingUpdate bandSetting(bandPath, bandName, SettingUpdate::Meaning::VALUE);
  applySettingUpdate(bandSetting);
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
  if (m_pControl != nullptr) {
    m_pControl->ptt(true);
  }
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
  if (m_pControl != nullptr) {
    m_pControl->ptt(false);
  }
  if (m_pReceiver != nullptr) {
    m_pReceiver->ptt(false);
  }
}

