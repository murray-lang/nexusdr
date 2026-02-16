//
// Created by murray on 27/07/25.
//

#pragma once

#include "ConfigBase.h"
#include "AudioConfig.h"
#include "ConfigException.h"
#include "ConfigFactory.h"
#include "IqIoConfig.h"

// Plain struct listing configuration members for programmer visibility
struct TransmitterConfigFields {
  IqIoConfig iqIo;
};

class TransmitterConfig : public ConfigBase, public TransmitterConfigFields
{
public:
  static constexpr auto type = "transmitter";
  explicit TransmitterConfig() : ConfigBase(type) {}

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("iqIo")) {
      iqIo.fromJson(json["iqIo"]);
    } else {
      throw ConfigException("TransmitterConfig: no iqIo configuration");
    }
  }

  // Convenience helpers to work with the plain struct form
  void setFields(const TransmitterConfigFields& f)
  {
    iqIo.setFields(f.iqIo);
  }

  [[nodiscard]] TransmitterConfigFields getFields() const
  {
    TransmitterConfigFields f;
    f.iqIo.setFields(iqIo.getFields());
    return f;
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
    return nlohmann::json{{"iqIo", iqIo.toJson()}};
  }
};