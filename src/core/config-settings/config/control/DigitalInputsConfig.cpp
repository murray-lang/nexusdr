#include "DigitalInputsConfig.h"

namespace Config::DigitalInputs
{
  static ResultCode factory(const TypedJson& json, DigitalInputConfigVariant& variant)
  {
    ResultCode result = ResultCode::OK;
    if (json.type == DigitalInput::type) {
      DigitalInput::Fields fields{};
      result = DigitalInput::fromJson(json.config, fields);

      if (result == ResultCode::OK) {
        variant = fields;
      }
      return result;
    }
    if (json.type == RotaryEncoder::type) {
      RotaryEncoder::Fields fields{};
      result = RotaryEncoder::fromJson(json.config, fields);

      if (result == ResultCode::OK) {
        variant = fields;
      }
      return result;
    }
    return ResultCode::ERR_CONFIG_UNKNOWN_TYPE;
  }

  ResultCode fromJson(const TypedJson& json, Fields& fields)
  {
    fields.type = type;
    fields.inputs.clear();

    ResultCode result = ResultCode::OK;
    if (json.config["inputs"]) {
      for (JsonVariantConst inputJson : json.config["inputs"].as<JsonArrayConst>()) {
        TypedJson typedJson;
        result = typedJson.fromJson(inputJson);

        if (result != ResultCode::OK) return result;

        DigitalInputConfigVariant inputOpt;
        result = factory(typedJson, inputOpt);

        if (result != ResultCode::OK) return result;

        fields.inputs.emplace_back(inputOpt);
      }
    }
    return ResultCode::OK;
  }

}