#include "DigitalOutputsConfig.h"

namespace Config::DigitalOutputs
{
  static ResultCode factory(const TypedJson& json, DigitalOutputConfigVariant& variant)
  {
    ResultCode result = ResultCode::OK;
    if (json.type == DigitalOutput::type) {
      DigitalOutput::Fields fields{};
      result = DigitalOutput::fromJson(json.config, fields);

      if (result == ResultCode::OK) {
        variant = fields;
      }
      return result;
    }
    if (json.type == DigitalOutput::BandSelector::type) {
      DigitalOutput::BandSelector::Fields fields{};
      result = DigitalOutput::BandSelector::fromJson(json.config, fields);

      if (result == ResultCode::OK) {
        variant = fields;
      }
      return result;
    }
    return ResultCode::UNKNOWN_TYPE;
  }

  ResultCode fromJson(const JsonDocument& json, Fields& fields)
  {
    fields.type = type;
    fields.outputs.clear();

    ResultCode result = ResultCode::OK;
    if (json["outputs"].is<JsonVariantConst>()) {
      for (JsonVariantConst inputJson : json["outputs"].as<JsonArrayConst>()) {
        TypedJson typedJson;
        result = typedJson.fromJson(inputJson);

        if (result != ResultCode::OK) return result;

        DigitalOutputConfigVariant inputOpt;
        result = factory(typedJson, inputOpt);

        if (result != ResultCode::OK) return result;

        fields.outputs.emplace_back(inputOpt);
      }
    }
    return ResultCode::OK;
  }

}