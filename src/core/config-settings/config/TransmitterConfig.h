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

  void fromJson(JsonVariantConst json) override
  {
    if (json["iqIo"]) {
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

  void toJson(JsonObject& json) const override
  {
    JsonObject iqIoObj = json["iqIo"].to<JsonObject>();
    iqIo.toJson(iqIoObj);
  }
};