//
// Created by murray on 2025-08-24.
//

#include <settings/control/RadioControl.h>

#include <settings/control/ControlException.h>
#include <settings/control/ControlSinkFactory.h>
#include <settings/control/ControlSourceFactory.h>
#include <config/ConfigException.h>

void
RadioControl::configure(const RadioControlConfig& config)
{
  const std::vector<ControlBaseConfig>& controlSinkConfigs = config.getSinks();
  for (auto& controllerConfig : controlSinkConfigs) {
    ControlSink* next = ControlSinkFactory::create(dynamic_cast<const ControlBaseConfig&>(controllerConfig));
    if (next != nullptr) {
      m_sinks.push_back(next);
    } else {
      std::ostringstream oss;
      oss << "Failed to create sink controller of type '" << controllerConfig.getType() << "'";
      throw ConfigException(oss.str());
    }
  }

  const std::vector<ControlBaseConfig>& controlSourceConfigs = config.getSources();
  for (auto& controllerConfig : controlSourceConfigs) {
    ControlSource* next = ControlSourceFactory::create(dynamic_cast<const ControlBaseConfig&>(controllerConfig));
    if (next != nullptr) {
      m_sources.push_back(next);
    } else {
//      std::ostringstream oss;
//      oss << "Failed to create source controller of type '" << controllerConfig.getType() << "'";
//      throw ConfigException(oss.str());
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
      oss << "Discovery failed for device controller: '" << pSink->getId() << "'";
      throw ControlException(oss.str());
    }
    pSink->open();
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
