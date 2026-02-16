//
// Created by murray on 2025-08-24.
//

#include "RadioControl.h"

#include "ControlException.h"
#include "ControlSinkFactory.h"
#include "ControlSourceFactory.h"
#include "../../config-settings/config/ConfigException.h"
#include "config-settings/settings/RadioSettings.h"
#include "config-settings/settings/base/SettingUpdatePath.h"

RadioControl::RadioControl() :
  m_internalSink(this),
  m_pExternalSettingsSink(nullptr)
{
}

void
RadioControl::configure(const ControlConfig* pConfig)
{
  const std::vector<ConfigBase*>& controlSinkConfigs = pConfig->getSinks();
  for (auto& controllerConfig : controlSinkConfigs) {
    ControlSink* next = ControlSinkFactory::create(controllerConfig);
    if (next != nullptr) {
      m_controlSinks.push_back(next);
    } else {
      std::ostringstream oss;
      oss << "Failed to create control sink of type '" << controllerConfig->getType() << "'";
      throw ConfigException(oss.str());
    }
  }

  const std::vector<ConfigBase*>& controlSourceConfigs = pConfig->getSources();
  for (auto& controllerConfig : controlSourceConfigs) {
    ControlSource* next = ControlSourceFactory::create(controllerConfig);
    if (next != nullptr) {
      next->connect(&m_internalSink);
      m_controlSources.push_back(next);
    } else {
      std::ostringstream oss;
      oss << "Failed to create control source of type '" << controllerConfig->getType() << "'";
      throw ConfigException(oss.str());
    }
  }
}

void
RadioControl::connect(RadioSettingsSink* pSink)
{
  m_pExternalSettingsSink = pSink;
}

void
RadioControl::notifySettings(const RadioSettings& radioSettings)
{
  if (m_pExternalSettingsSink) {
    m_pExternalSettingsSink->applySettings(radioSettings);
  }
}

void
RadioControl::notifySettingUpdate(SettingUpdate& settingDelta)
{
  if (m_pExternalSettingsSink) {
    m_pExternalSettingsSink->applySettingUpdate(settingDelta);
  }
}

void
RadioControl::applySettings(const RadioSettings& settings)
{
  for (RadioSettingsSink* pSink : m_controlSinks) {
    pSink->applySettings(settings);
  }
}

void
RadioControl::applySettingUpdate(SettingUpdate& setting)
{
  for (RadioSettingsSink* pSink : m_controlSinks) {
    pSink->applySettingUpdate(setting);
  }
}

void
RadioControl::start()
{
  for (auto pSink : m_controlSinks) {
    if (!pSink->discover()) {
      std::ostringstream oss;
      oss << "Discovery failed for control sink: '" << pSink->getId() << "'";
      throw ControlException(oss.str());
    }
    pSink->open();
  }
  for (auto pSource : m_controlSources) {
    if (!pSource->discover()) {
      std::ostringstream oss;
      oss << "Discovery failed for control source: '" << pSource->getId() << "'";
      throw ControlException(oss.str());
    }
    pSource->open();
  } 
}

void
RadioControl::stop()
{

}

void
RadioControl::ptt(bool on)
{
  SettingUpdatePath path({RadioSettings::PTT});
  SettingUpdate setting(path, on, SettingUpdate::VALUE);
  applySettingUpdate(setting);
}
