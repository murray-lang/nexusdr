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
struct ReceiverConfigFields {
  IqIoConfig iqIo;
};

class ReceiverConfig : public ConfigBase, public ReceiverConfigFields
{
public:
  static constexpr auto type = "receiver";
  explicit ReceiverConfig() : ConfigBase(type) {}

  void fromJson(const nlohmann::json& json) override
  {
    if (json.contains("iqIo")) {
      iqIo.fromJson(json["iqIo"]);
    } else {
      throw ConfigException("ReceiverConfig: no iqIo configuration");
    }
  }

  // Convenience helpers to work with the plain struct form
  void setFields(const ReceiverConfigFields& f)
  {
    iqIo.setFields(f.iqIo);
  }

  [[nodiscard]] ReceiverConfigFields getFields() const
  {
    ReceiverConfigFields f;
    f.iqIo.setFields(iqIo.getFields());
    return f;
  }

  [[nodiscard]] nlohmann::json toJson() const override
  {
   return nlohmann::json{{"iqIo", iqIo.toJson()}};
  }
};
