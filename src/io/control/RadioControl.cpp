#include "RadioControl.h"

#include "ControlSinkFactory.h"
#include "ControlSourceFactory.h"
#include "core/config-settings/settings/RadioSettings.h"
#include "core/config-settings/settings/base/SettingUpdatePath.h"

// #ifdef USE_ETL
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
      m_controlSinks.emplace_back(move(sink));
    } else {
      return rc;
    }
  }

  for (auto& controllerConfig : config.sources) {
    ControlSourceVariant source;
    rc = ControlSourceFactory::create(controllerConfig, source);
    if (rc == ResultCode::OK) {
      rc = visit([this](auto&& s) -> ResultCode {
        using T = decay_t<decltype(s)>;
        if (!is_same_v<T, monostate>) {
          s.connect(&m_internalSink);
          m_controlSources.emplace_back(move(s));
          return ResultCode::OK;
        } else
        {
          return ResultCode::ERR_NO_CONTROL_SOURCES_DEFINED;
        }
      }, source);
      if (rc != ResultCode::OK) {
        break;
      }
    } else {
      return rc ; //ResultCode::ERR_CONTROL_SOURCE_NOT_FOUND;
    }
  }
  return rc;
}

void
RadioControl::connect(RadioSettingsSink* pSink)
{
  m_pExternalSettingsSink = pSink;
}

ResultCode
RadioControl::notifySettings(const RadioSettings& radioSettings)
{
  if (m_pExternalSettingsSink) {
    return m_pExternalSettingsSink->applySettings(radioSettings);
  }
  return ResultCode::OK;
}

ResultCode
RadioControl::notifySettingUpdate(SettingUpdate& settingDelta)
{
  if (m_pExternalSettingsSink) {
    return m_pExternalSettingsSink->applySettingUpdate(settingDelta);
  }
  return ResultCode::OK;
}

ResultCode
RadioControl::applySettings(const RadioSettings& settings)
{
  for (auto& sinkVar : m_controlSinks) {
    const ResultCode rc = visit([&settings] (auto&& sink) -> ResultCode
    {
      return sink.applySettings(settings);
    }, sinkVar);
    if (rc != ResultCode::OK) {
      return rc;
    }
  }
  return ResultCode::OK;
}

ResultCode
RadioControl::applySettingUpdate(SettingUpdate& setting)
{
  for (auto& sinkVar : m_controlSinks) {
    const ResultCode rc = visit([&setting] (auto&& sink) -> ResultCode
    {
      return sink.applySettingUpdate(setting);
    }, sinkVar);
    if (rc != ResultCode::OK) {
      return rc;
    }
  }
  return ResultCode::OK;
}

ResultCode
RadioControl::start()
{
  for (auto& pSink : m_controlSinks) {
    ResultCode rc = visit([&pSink](auto&& sink) -> ResultCode
    {
      using T = decay_t<decltype(sink)>;
      if constexpr (is_same_v<T, monostate>) {
        return ResultCode::ERR_CONTROL_NO_SINKS_AVAILABLE;
      } else {
        if (sink.discover()) {
          return sink.open();
        }
      }
      return ResultCode::ERR_CONTROL_SINK_DISCOVER;
    }, pSink);
    if (rc != ResultCode::OK) {
      return rc;
    }
  }
  for (auto& pSource : m_controlSources) {
    ResultCode rc = visit([&pSource](auto&& source) -> ResultCode
    {
      using T = decay_t<decltype(source)>;
      if constexpr (is_same_v<T, monostate>) {
        return ResultCode::ERR_CONTROL_NO_SOURCES_AVAILABLE;
      } else {
        if (source.discover()) {
          return source.open();
        }
        return ResultCode::ERR_CONTROL_SOURCE_DISCOVER;
      }
    }, pSource);
    if (rc != ResultCode::OK) {
      return rc;
    }

  }
  return ResultCode::OK;
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
