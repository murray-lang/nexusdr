#include "Radio.h"
#include "receiver/IqReceiver.h"
#include "transmitter/IqTransmitter.h"
#include <io/control/RadioControl.h>

#include "core/config-settings/settings/events/RadioSettingsEvent.h"



using RadioConfig = Config::Radio::Fields;

Radio::Radio(EventTarget *pEventTarget) :
  RadioBase(pEventTarget),
  m_receiver(nullopt),
  m_transmitter(nullopt)
{

}

Radio::~Radio()
{
}

ResultCode
Radio::configure(const Config::Radio::Fields& config)
{
  ResultCode rc = m_control.configure(config.control);

  if (rc != ResultCode::OK) return rc;

  if (config.receiver) {
    m_receiver.emplace(m_pEventTarget);
    rc = m_receiver->configure(*config.receiver);
    if (rc != ResultCode::OK) return rc;
  }
  if (config.transmitter) {
    m_transmitter.emplace(m_pEventTarget);
    rc = m_transmitter->configure(*config.transmitter);
  }
  return rc;
}

void
Radio::start()
{
  m_control.connect(this);
  if (m_receiver) {
    m_receiver->start();
  }
  m_control.start();
}

void
Radio::stop()
{
  m_control.stop();
  m_control.connect(nullptr);
  if (m_receiver) {
    m_receiver->stop();
  }
  if (m_transmitter) {
    m_transmitter->stop();
  }
}

ResultCode
Radio::applySettings(const RadioSettings& settings)
{
  if (&settings != &m_settings) {
    m_settings = settings;
  }
  if (settings.hasSettingChanged(RadioSettings::PTT)) {
    ptt(m_settings.getPtt());
    m_settings.clearChanged();
    return ResultCode::OK; // Don't try to do anything else concurrently with PTT.
  }

  if (m_settings.hasSettingChanged(RadioSettings::BAND)) {

    BandSettings* pBandSettings = m_settings.getFocusBandSettings();
    // RxPipelineSettings* rxPipelineSettings = pBandSettings->getFocusPipeline();
    if (m_receiver) {
      // m_pReceiver->adjustRfSettingsToLimits(rxPipelineSettings->getRfSettings());
      m_receiver->apply(pBandSettings);
    }
    TxPipelineSettings* txPipelineSettings = pBandSettings->getTxPipeline();
    if (m_transmitter) {
      m_transmitter->adjustRfSettingsToLimits(txPipelineSettings->getRfSettings());
      m_transmitter->apply(txPipelineSettings);
    }
  }
  m_control.applySettings(m_settings);

  if (settings.hasSettingChanged(RadioSettings::RX)) {
    if (m_receiver) {
      m_receiver->apply(m_settings.getRxSettings());
    }
  }
  if (settings.hasSettingChanged(RadioSettings::TX)) {
    if (m_transmitter) {
      m_transmitter->apply(m_settings.getTxSettings());
    }
  }
  if (m_pEventTarget != nullptr) {
    auto* rse = new RadioSettingsEvent(m_settings, ++m_updateSequenceNo, SettingEventBase::BACK_END);
    EventDispatcher::postEvent(m_pEventTarget, rse);
  }
  m_settings.clearChanged();
  return ResultCode::OK;
}

ResultCode
Radio::applySettingUpdate(SettingUpdate& update)
{
  return RadioBase::applySettingUpdate(update);
}

void
Radio::markAllSettingsUnchanged()
{
  m_settings.clearChanged();
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
  if (m_receiver) {
    m_receiver->ptt(true);
  }
  m_control.ptt(true);

  if (m_transmitter) {
    m_transmitter->ptt(true);
  }
}

void
Radio::pttOff()
{
  if (m_transmitter) {
    m_transmitter->ptt(true);
  }
  if (m_receiver) {
    m_receiver->ptt(true);
  }
  m_control.ptt(true);
}