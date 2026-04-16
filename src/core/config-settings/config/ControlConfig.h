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

  void fromJson(JsonVariantConst json) override
  {
    if (json["sinks"]) {
      for (JsonVariantConst sinkConfig : json["sinks"].as<JsonArrayConst>()) {
        VariantConfig variantConfig(sinkConfig);
        ConfigBase* config = ConfigFactory::create(variantConfig);
        m_sinks.push_back(config);
      }
    }
    if (json["sources"]) {
      for (JsonVariantConst sourceConfig : json["sources"].as<JsonArrayConst>()) {
        VariantConfig variantConfig(sourceConfig);
        ConfigBase* config = ConfigFactory::create(variantConfig);
        m_sources.push_back(config);
      }
    }
  }

  void toJson(JsonObject& json) const override
  {
    JsonArray sinks = json["sinks"].to<JsonArray>();
    for (const auto* s : m_sinks) {
      if (s) {
        JsonObject sinkObj = sinks.add<JsonObject>();
        sinkObj["type"] = s->getType();
        JsonObject config = sinkObj["config"].to<JsonObject>();
        s->toJson(config);
      }
    }
    JsonArray sources = json["sources"].to<JsonArray>();
    for (const auto* s : m_sources) {
      if (s) {
        JsonObject sourceObj = sources.add<JsonObject>();
        sourceObj["type"] = s->getType();
        JsonObject config = sourceObj["config"].to<JsonObject>();
        s->toJson(config);
      }
    }
  }

  [[nodiscard]] const std::vector<ConfigBase*>& getSinks() const { return m_sinks; }
  [[nodiscard]] const std::vector<ConfigBase*>& getSources() const { return m_sources; }

protected:
  std::vector<ConfigBase*> m_sinks;
  std::vector<ConfigBase*> m_sources;
};
