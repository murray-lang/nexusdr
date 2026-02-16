//
// Created by murray on 18/08/25.
//

#include "Radio.h"

#include <qcoreapplication.h>

#include "config-settings/settings/RadioSettings.h"
#include "config-settings/settings/RadioSettingsEvent.h"
#include <QDebug>


Radio::Radio(QObject *pEventTarget) :
  m_settings(),
  m_pReceiver(nullptr),
  m_pTransmitter(nullptr),
  m_pControl(nullptr),
  m_pEventTarget(pEventTarget),
  m_updateSequence(0)
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


  if (settings.hasSettingChanged(RadioSettings::BAND)) {

    BandSettings* pBandSettings = settings.getFocusBandSettings();
    // RxPipelineSettings* rxPipelineSettings = pBandSettings->getFocusPipeline();
    if (m_pReceiver != nullptr) {
      // m_pReceiver->adjustRfSettingsToLimits(rxPipelineSettings->getRfSettings());
      m_pReceiver->apply(pBandSettings);
    }
    TxPipelineSettings* txPipelineSettings = pBandSettings->getTxPipeline();
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
    RadioSettingsEvent* rse = new RadioSettingsEvent(settings, ++m_updateSequence);
    QCoreApplication::postEvent(m_pEventTarget, rse);
  }
  m_settings.clearChanged();
  // pBandSettings->clearChanged();
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

void Radio::applySettingUpdates(SettingUpdate* updates, std::size_t count)
{
  if (!updates) return;

  bool anyChanged = false;

  for (std::size_t i = 0; i < count; ++i) {
    updates[i].resetCursor();
    anyChanged |= m_settings.applyUpdate(updates[i]);
  }

  if (anyChanged) {
    applySettings(m_settings);
  }
}

void
Radio::applyBand(const std::string& bandName)
{
  // qDebug() << "Radio::applyBand(): applying band " << bandName.c_str() << ". Existing band: " << m_settings.bandName.c_str() ;
  SettingUpdatePath bandPath({RadioSettings::BAND, BandSelector::REPLACE_FOCUS});
  SettingUpdate bandSetting(bandPath, bandName, SettingUpdate::Meaning::VALUE);
  applySettingUpdate(bandSetting);
}

void
Radio::split(const std::string& bandA, const std::string& bandB)
{
  SettingUpdatePath splitPath({RadioSettings::BAND, BandSelector::SPLIT});
  SettingUpdate splitSetting(splitPath, true, SettingUpdate::Meaning::VALUE);
  applySettingUpdate(splitSetting);
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

