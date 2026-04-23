#include "ControlConfig.h"

namespace Config::Control
{

  static Result SinkFactory(const TypedJson& json, SinkVariant& sink)
  {
    Result result = Result::OK;
    if (json.type == DigitalOutput::type) {
      DigitalOutput::Fields fields{};
      result = DigitalOutput::fromJson(json.config, fields);
      if (result == Result::OK) {
        sink = fields;
      }
      return result;
    }
    if (json.type == FunCube::type) {
      FunCube::Fields fields{};
      result = FunCube::fromJson(json.config, fields);
      if (result == Result::OK) {
        sink = fields;
      }
      return result;
    }
    if (json.type == DigitalOutput::BandSelector::type) {
      DigitalOutput::BandSelector::Fields fields{};
      result = DigitalOutput::BandSelector::fromJson(json.config, fields);
      if (result == Result::OK) {
        sink = fields;
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  static Result SourceFactory(const TypedJson& json, SourceVariant& source)
  {
    Result result = Result::OK;
    if (json.type == DigitalInputs::type) {
      DigitalInputs::Fields fields{};
      result = DigitalInputs::fromJson(json, fields);
      if (result == Result::OK) {
        source = fields;
      }
      return result;
    }
    return Result::UNKNOWN_TYPE;
  }

  Result fromJson(const TypedJson& json, Fields& fields)
  {
    fields.sinks.clear();
    fields.sources.clear();

    Result result = Result::OK;
    if (json.config["sinks"]) {
      for (JsonVariantConst sinkJson : json.config["sinks"].as<JsonArrayConst>()) {
        TypedJson taggedJson;
        result = taggedJson.fromJson(sinkJson);

        if (result != Result::OK) return result;

        SinkVariant sink;
        result = SinkFactory(taggedJson, sink);

        if (result != Result::OK) return result;

        fields.sinks.emplace_back(sink);
      }
    }
    if (json.config["sources"]) {
      for (JsonVariantConst sourceJson : json.config["sources"].as<JsonArrayConst>()) {
        TypedJson taggedJson;
        result = taggedJson.fromJson(sourceJson);

        if (result != Result::OK) return result;

        SourceVariant source;
        result = SourceFactory(taggedJson, source);

        if (result != Result::OK) return result;

        fields.sources.emplace_back(source);
      }
    }
    return Result::OK;
  }

}