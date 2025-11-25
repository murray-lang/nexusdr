//
// Created by murray on 20/8/25.
//

#pragma once

#include "VariantConfig.h"
#include "ConfigBase.h"
#include "ConfigFactory.h"
#include <vector>


// Plain struct listing configuration members for programmer visibility
struct ControlConfigFields {
  std::vector<VariantConfig> sinks;
  std::vector<VariantConfig> sources;
};

class ControlConfig : public ConfigBase, public ControlConfigFields
{
  // friend class RadioConfig;
public:
  static constexpr auto type = "control";

  ControlConfig() : ConfigBase(type) {}
  // RadioControlConfig(const RadioControlConfig& rhs) : m_sinks(rhs.m_sinks), m_sources(rhs.m_sources)
  // {
  // }
  ~ControlConfig() override
  {
    for (auto p : m_sinks) {
      delete p;
    }
    m_sinks.clear();
    for (auto p : m_sources) {
      delete p;
    }
    m_sources.clear();
  }

  // RadioControlConfig& operator=(const RadioControlConfig& rhs)
  // {
  //   m_sinks = rhs.m_sinks;
  //   m_sources = rhs.m_sources;
  //   return *this;
  // }

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("sinks")) {
      for (auto& sinkConfig : json["sinks"]) {
        VariantConfig variantConfig(sinkConfig);
        ConfigBase* config = ConfigFactory::create(variantConfig);
        m_sinks.push_back(config);
      }
    }
    if (json.contains("sources")) {
      for (auto& sourceConfig : json["sources"]) {
        VariantConfig variantConfig(sourceConfig);
        ConfigBase* config = ConfigFactory::create(variantConfig);
        m_sources.push_back(config);
      }
    }
  }

  // Convenience helpers to work with the plain struct form
  void setFields(const ControlConfigFields& f)
  {
    static_cast<ControlConfigFields&>(*this) = f;
    for (auto* p : m_sinks) delete p; m_sinks.clear();
    for (auto* p : m_sources) delete p; m_sources.clear();
    for (const auto& vc : f.sinks) m_sinks.push_back(ConfigFactory::create(vc));
    for (const auto& vc : f.sources) m_sources.push_back(ConfigFactory::create(vc));
  }

  [[nodiscard]] ControlConfigFields getFields() const
  {
    ControlConfigFields f;
    for (const auto* s : m_sinks) if (s) f.sinks.emplace_back(nlohmann::json{{"type", s->getType()}, {"config", s->toJson()}});
    for (const auto* s : m_sources) if (s) f.sources.emplace_back(nlohmann::json{{"type", s->getType()}, {"config", s->toJson()}});
    return f;
  }
  [[nodiscard]] nlohmann::json toJson() const override
  {
    nlohmann::json sinks = nlohmann::json::array();
    for (const auto* s : m_sinks) {
      if (s) sinks.push_back(nlohmann::json{{"type", s->getType()}, {"config", s->toJson()}});
    }
    nlohmann::json sources = nlohmann::json::array();
    for (const auto* s : m_sources) {
      if (s) sources.push_back(nlohmann::json{{"type", s->getType()}, {"config", s->toJson()}});
    }
    return nlohmann::json{{"sinks", sinks}, {"sources", sources}};
  }

  [[nodiscard]] const std::vector<ConfigBase*>& getSinks() const { return m_sinks; }
  [[nodiscard]] const std::vector<ConfigBase*>& getSources() const { return m_sources; }

protected:
  std::vector<ConfigBase*> m_sinks;
  std::vector<ConfigBase*> m_sources;
};
