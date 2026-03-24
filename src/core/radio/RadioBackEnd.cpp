//
// Created by murray on 23/3/26.
//

#include "RadioBackEnd.h"
#include "receiver/IqReceiver.h"
#include "transmitter/IqTransmitter.h"
#include <io/control/RadioControl.h>

RadioBackEnd::RadioBackEnd(EventTarget *pEventTarget) :
  RadioBase(pEventTarget),
  m_pReceiver(nullptr),
  m_pTransmitter(nullptr),
  m_pControl(nullptr)
{

}

RadioBackEnd::~RadioBackEnd()
{
  delete m_pReceiver;
  delete m_pTransmitter;
  delete m_pControl;
}

void
RadioBackEnd::configure(const RadioConfig* pConfig)
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
RadioBackEnd::start()
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
RadioBackEnd::stop()
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
RadioBackEnd::applySettings(const RadioSettings& settings)
{
  if (&settings != &m_settings) {
    m_settings = settings;
  }
  if (settings.hasSettingChanged(RadioSettings::PTT)) {
    ptt(m_settings.getPtt());
    m_settings.clearChanged();
    return; // Don't try to do anything else concurrently with PTT.
  }

  if (m_settings.hasSettingChanged(RadioSettings::BAND)) {

    BandSettings* pBandSettings = m_settings.getFocusBandSettings();
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
    auto* rse = new RadioSettingsEvent(m_settings, ++m_updateSequenceNo);
    EventDispatcher::postEvent(m_pEventTarget, rse);
  }
  m_settings.clearChanged();
}

void
RadioBackEnd::applySettingUpdate(SettingUpdate& update)
{

}

void
RadioBackEnd::ptt(bool on)
{
  if (on) {
    pttOn();
  } else {
    pttOff();
  }
}

void
RadioBackEnd::pttOn()
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
RadioBackEnd::pttOff()
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