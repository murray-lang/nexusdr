//
// Created by murray on 2025-08-24.
//

#include "RadioControl.h"

#include "ControlException.h"
#include "ControlSinkFactory.h"
#include "ControlSourceFactory.h"
#include "config/ConfigException.h"

void
RadioControl::configure(const ControlConfig* pConfig)
{
  const std::vector<ConfigBase*>& controlSinkConfigs = pConfig->getSinks();
  for (auto& controllerConfig : controlSinkConfigs) {
    ControlSink* next = ControlSinkFactory::create(controllerConfig);
    if (next != nullptr) {
      m_sinks.push_back(next);
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
      m_sources.push_back(next);
    } else {
      std::ostringstream oss;
      oss << "Failed to create control source of type '" << controllerConfig->getType() << "'";
      throw ConfigException(oss.str());
    }
  }
}

void
RadioControl::apply(const RadioSettings& settings)
{
  for (auto pSink : m_sinks) {
    pSink->apply(settings);
  }
}

void
RadioControl::start()
{
  for (auto pSink : m_sinks) {
    if (!pSink->discover()) {
      std::ostringstream oss;
      oss << "Discovery failed for control sink: '" << pSink->getId() << "'";
      throw ControlException(oss.str());
    }
    pSink->open();
  }
  for (auto pSource : m_sources) {
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

}
