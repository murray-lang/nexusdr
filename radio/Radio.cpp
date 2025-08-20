//
// Created by murray on 18/08/25.
//

#include "Radio.h"

#include "../config/ConfigException.h"
#include "../io/controller/SinkControllerFactory.h"
#include "../io/controller/SourceControllerFactory.h"
#include "../io/controller/ControllerException.h"


#define SAMPLE_RATE 192000

Radio::Radio(QObject *eventTarget)
{
  m_pReceiver = new IqReceiver(eventTarget);
}

Radio::~Radio()
{
  delete m_pReceiver;
}

void
Radio::configure(const RadioConfig& config)
{
  m_pReceiver->configure(config.getReceiver());
  const std::vector<ControllerConfig>& controlSinkConfigs = config.getControl().getSinks();
  for (auto& controllerConfig : controlSinkConfigs) {
    SinkController* next = SinkControllerFactory::create(controllerConfig);
    if (next != nullptr) {
      m_controlSinks.push_back(next);
    } else {
      std::ostringstream oss;
      oss << "Failed to create sink controller of type '" << controllerConfig.getType() << "'";
      throw ConfigException(oss.str());
    }
  }

  const std::vector<ControllerConfig>& controlSourceConfigs = config.getControl().getSources();
  for (auto& controllerConfig : controlSourceConfigs) {
    SourceController* next = SourceControllerFactory::create(controllerConfig);
    if (next != nullptr) {
      m_controlSources.push_back(next);
    } else {
      std::ostringstream oss;
      oss << "Failed to create source controller of type '" << controllerConfig.getType() << "'";
      throw ConfigException(oss.str());
    }
  }
}

void
Radio::start() const
{
  for (auto pDeviceController : m_controlSinks) {
    if (!pDeviceController->discover()) {
      std::ostringstream oss;
      oss << "Discovery failed for device controller: '" << pDeviceController->getId() << "'";
      throw ControllerException(oss.str());
    }
    pDeviceController->open();
  }
  m_pReceiver->start();
}

void
Radio::stop() const
{
  m_pReceiver->stop();
}

void
Radio::apply(const RadioSettings& settings)
{
  for (auto& deviceController : m_controlSinks) {
    deviceController->apply(settings);
  }
  m_pReceiver->apply(settings.rxSettings);
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
  for (auto& controlSink : m_controlSinks) {
    controlSink->ptt(true);
  }
}

void
Radio::pttOff()
{
  for (auto& controlSink : m_controlSinks) {
    controlSink->ptt(false);
  }
  if (m_pReceiver != nullptr) {
    m_pReceiver->ptt(false);
  }
}

