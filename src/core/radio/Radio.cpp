//
// Created by murray on 23/3/26.
//

#include "Radio.h"
#include "receiver/IqReceiver.h"
#include "transmitter/IqTransmitter.h"
#include <io/control/RadioControl.h>

#include "core/config-settings/settings/events/RadioSettingsEvent.h"



using RadioConfig = Config::Radio::Fields;

Radio::Radio(EventTarget *pEventTarget) :
  RadioBase(pEventTarget),
  m_pReceiver(nullptr),
  m_pTransmitter(nullptr),
  m_pControl(nullptr)
{

}

Radio::~Radio()
{
  delete m_pReceiver;
  delete m_pTransmitter;
  delete m_pControl;
}

ResultCode
Radio::configure(const RadioConfig* pConfig)
{
  ResultCode rc = ResultCode::OK;
  const ControlConfig* pControlConfig = &pConfig->control;
  if (pControlConfig != nullptr) {
    m_pControl = new RadioControl();
    rc = m_pControl->configure(pControlConfig);
  }
  const ReceiverConfig* pRxConfig = &pConfig->receiver;
  if (pRxConfig != nullptr) {
    m_pReceiver = new IqReceiver(m_pEventTarget);
    m_pReceiver->configure(pRxConfig);
  }

  const TransmitterConfig* pTxConfig = &pConfig->transmitter;
  if (pTxConfig != nullptr) {
    m_pTransmitter = new IqTransmitter(m_pEventTarget);
    m_pTransmitter->configure(pTxConfig);
  }
  return rc;
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
    auto* rse = new RadioSettingsEvent(m_settings, ++m_updateSequenceNo, SettingEventBase::BACK_END);
    EventDispatcher::postEvent(m_pEventTarget, rse);
  }
  m_settings.clearChanged();
}

void
Radio::applySettingUpdate(SettingUpdate& update)
{
  RadioBase::applySettingUpdate(update);
}

void
Radio::ptt(bool on)
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