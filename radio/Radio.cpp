//
// Created by murray on 18/08/25.
//

#include "Radio.h"

#include <qcoreapplication.h>

#include "settings/RadioSettings.h"
#include "settings/RadioSettingsEvent.h"
#include "settings/SingleSettingEvent.h"
#include <QDebug>


Radio::Radio(QObject *pEventTarget) :
  m_settings(),
  m_pReceiver(nullptr),
  m_pTransmitter(nullptr),
  m_pControl(nullptr),
  m_pEventTarget(pEventTarget)
{
  initialiseBandSettings();
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
Radio::initialiseBandSettings()
{
  // These will eventually be overridden by settings from JSON
  const std::vector<BandCategory>& categories = m_bands.getCategories();
  for (const auto& category : categories) {
    for (const auto& band : category.getBands()) {
      m_bandSettings.emplace(band.getName(), BandSettings(band));
    }
  }
}

BandSettings*
Radio::getBandSettings(const std::string& bandName)
{
  if (!m_bandSettings.contains(bandName)) {
    if (const Band* bandInfo = m_bands.findBand(bandName)) {
      m_bandSettings.emplace(bandName, BandSettings(*bandInfo));
    } else {
      return nullptr;
    }
  }
  return &m_bandSettings.at(bandName);
}

void
Radio::applyRfSettings(const RfSettings& settings)
{
  for (auto& item : m_bandSettings) {
    item.second.applyRfSettings(settings);
  }
}

void
Radio::applyIfSettings(const IfSettings& settings)
{
  for (auto& item : m_bandSettings) {
    item.second.applyIfSettings(settings);
  }
}

void
Radio::applySettings(const RadioSettings& settings) {


  BandSettings* pBandSettings = getBandSettings(settings.bandName);
  if (pBandSettings != nullptr) {
    
    applySettings(settings, pBandSettings);
  }
}

void
Radio::applySettings(const RadioSettings& settings, BandSettings* pBandSettings)
{
  if (&settings != &m_settings) {
    m_settings = settings;
  }
  if (settings.changed & RadioSettings::PTT) {
    ptt(m_settings.ptt);
    m_settings.clearChanged();
    return; // Don't try to do anything else concurrently with PTT.
  }

  if (settings.changed & (RadioSettings::PIPELINE | RadioSettings::BAND)) {
    RxPipelineSettings* rxPipelineSettings = pBandSettings->getFocusRxPipelineSettings();
    if (m_pReceiver != nullptr) {
      m_pReceiver->apply(rxPipelineSettings);
    }
    TxPipelineSettings* txPipelineSettings = pBandSettings->getTxPipelineSettings();
    if (m_pTransmitter != nullptr) {
      m_pTransmitter->apply(txPipelineSettings);
    }
  }
  if (m_pControl != nullptr) {
    m_pControl->applySettings(m_settings, pBandSettings);
  }

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
    QCoreApplication::postEvent(m_pEventTarget, new RadioSettingsEvent(settings, *pBandSettings));
  }
  m_settings.clearChanged();
  pBandSettings->clearChanged();
}

void
Radio::applySingleSetting(const SingleSetting& setting)
{
  if (m_pEventTarget != nullptr) {
    // QCoreApplication::postEvent(m_pEventTarget, new SingleSettingEvent(setting));
  }

  // Intercept BAND changes so that any change can be detected and the new band settings marked as all changed to force updates
  if (setting.getPath().getFeatures()[0] == RadioSettings::Features::BAND) {
    std::string newBandName = std::get<std::string>(setting.getValue());
    if (m_settings.bandName != newBandName) {
      BandSettings* pBandSettings = getBandSettings(newBandName);
      if (pBandSettings != nullptr) {
        pBandSettings->setAllChanged();
      }
    }
  }

  if (m_settings.applySetting(setting, 0)) {
    applySettings(m_settings);
  } else if (setting.getPath().getFeatures()[0] == RadioSettings::Features::PIPELINE) {
    BandSettings* pBandSettings = getBandSettings(m_settings.bandName);
    if (pBandSettings != nullptr) {
      if (pBandSettings->applySetting(setting, 1)) {
        m_settings.changed |= RadioSettings::PIPELINE;
        applySettings(m_settings, pBandSettings);
        pBandSettings->clearChanged();
      }
    }
  }
}

void
Radio::applyBand(const std::string& bandName)
{
  // qDebug() << "Radio::applyBand(): applying band " << bandName.c_str() << ". Existing band: " << m_settings.bandName.c_str() ;
  SettingPath bandPath({RadioSettings::Features::BAND});
  SingleSetting bandSetting(bandPath, bandName, SingleSetting::Meaning::VALUE);
  applySingleSetting(bandSetting);
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

