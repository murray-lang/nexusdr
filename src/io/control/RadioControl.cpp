//
// Created by murray on 2025-08-24.
//

#include "RadioControl.h"

#include "ControlSinkFactory.h"
#include "ControlSourceFactory.h"
#include "core/config-settings/settings/RadioSettings.h"
#include "core/config-settings/settings/base/SettingUpdatePath.h"

// #ifdef USE_ETL_COLLECTIONS
// #include "etl/visitor.h"
// #else
#include <variant>
#include <type_traits>
// #endif

RadioControl::RadioControl() :
  m_internalSink(this),
  m_pExternalSettingsSink(nullptr)
{
}

ResultCode
RadioControl::configure(const Config::Control::Fields& config)
{
  ResultCode rc = ResultCode::OK;
  for (auto& controllerConfig : config.sinks) {
    ControlSinkVariant sink;
    rc = ControlSinkFactory::create(controllerConfig, sink);
    if (rc == ResultCode::OK) {
      m_controlSinks.emplace_back(sink);
    } else {
      return rc;
    }
  }

  for (auto& controllerConfig : config.sources) {
    ControlSourceVariant source;
    rc = ControlSourceFactory::create(controllerConfig, source);
    if (rc == ResultCode::OK) {
      std::visit([this, rc](auto&& s) {
        using T = std::decay_t<decltype(s)>;
        if constexpr (!std::is_same_v<T, std::monostate>) {
          s.connect(&m_internalSink);
        }
      }, source);
      if (!std::holds_alternative<std::monostate>(source)) {
        m_controlSources.emplace_back(source);
      } else {
        return ResultCode::ERR_NO_CONTROL_SOURCES_DEFINED;
      }
    } else {
      return ResultCode::ERR_CONFIG_CONTROL_SOURCE;
    }
  }
  return rc;
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
