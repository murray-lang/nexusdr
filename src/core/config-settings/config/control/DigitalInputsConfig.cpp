#include "DigitalInputsConfig.h"

namespace Config::DigitalInputs
{
  static Result factory(const TypedJson& json, OptionalVariant& optionalVariant)
  {
    Result result = Result::OK;
    if (json.type == DigitalInput::type) {
      DigitalInput::Fields fields{};
      result = DigitalInput::fromJson(json.config, fields);

      if (result == Result::OK) {
        optionalVariant.emplace(fields);
      }
      return result;
    }
    if (json.type == RotaryEncoder::type) {
      RotaryEncoder::Fields fields{};
      result = RotaryEncoder::fromJson(json.config, fields);

      if (result == Result::OK) {
        optionalVariant.emplace(fields);
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  Result fromJson(const TypedJson& json, Fields& fields)
  {
    fields.type = type;
    fields.inputs.clear();

    Result result = Result::OK;
    if (json.config["inputs"]) {
      for (JsonVariantConst inputJson : json.config["inputs"].as<JsonArrayConst>()) {
        TypedJson typedJson;
        result = typedJson.fromJson(inputJson);

        if (result != Result::OK) return result;

        OptionalVariant inputOpt;
        result = factory(typedJson, inputOpt);

        if (result != Result::OK) return result;

        fields.inputs.emplace_back(inputOpt);
      }
    }
    return Result::OK;
  }

}