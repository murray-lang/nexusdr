#include "ControlConfig.h"

namespace Config::Control
{

  static ResultCode SinkFactory(const TypedJson& json, SinkConfigVariant& sink)
  {
    ResultCode result = ResultCode::OK;
#ifdef USE_GPIO
    if (json.type == DigitalOutputs::type) {
      DigitalOutputs::Fields fields{};
      result = DigitalOutputs::fromJson(json.config, fields);
      if (result == ResultCode::OK) {
        sink = fields;
      }
      return result;
    }
#endif
#ifdef IS_QT
    if (json.type == QtControlSink::type) {
      QtControlSink::Fields fields{};
      result = QtControlSink::fromJson(json.config, fields);
      if (result == ResultCode::OK) {
        sink = fields;
      }
      return result;
    }
#endif
    if (json.type == FunCube::type) {
      FunCube::Fields fields{};
      result = FunCube::fromJson(json.config, fields);
      if (result == ResultCode::OK) {
        sink = fields;
      }
      return result;
    }
    // if (json.type == DigitalOutput::BandSelector::type) {
    //   DigitalOutput::BandSelector::Fields fields{};
    //   result = DigitalOutput::BandSelector::fromJson(json.config, fields);
    //   if (result == ResultCode::OK) {
    //     sink = fields;
    //   }
    //   return result;
    // }
    return ResultCode::ERR_CONFIG_UNKNOWN_TYPE;
  }

  static ResultCode SourceFactory(const TypedJson& json, SourceConfigVariant& source)
  {
    ResultCode result = ResultCode::OK;
#ifdef USE_GPIO
    if (json.type == DigitalInputs::type) {
      DigitalInputs::Fields fields{};
      result = DigitalInputs::fromJson(json, fields);
      if (result == ResultCode::OK) {
        source = fields;
      }
      return result;
    }
#endif
#ifdef IS_QT
    if (json.type == QtControlSource::type) {
      QtControlSource::Fields fields{};
      result = QtControlSource::fromJson(json.config, fields);
      if (result == ResultCode::OK) {
        source = fields;
      }
      return result;
    }
#endif
    return ResultCode::ERR_CONFIG_UNKNOWN_TYPE;
  }

  ResultCode fromJson(const JsonVariantConst& json, Fields& fields)
  {
    fields.sinks.clear();
    fields.sources.clear();

    ResultCode result = ResultCode::OK;
    if (json["sinks"].is<JsonVariantConst>()) {
      for (JsonVariantConst sinkJson : json["sinks"].as<JsonArrayConst>()) {
        TypedJson taggedJson;
        result = taggedJson.fromJson(sinkJson);

        if (result != ResultCode::OK) return result;

        SinkConfigVariant sink;
        result = SinkFactory(taggedJson, sink);

        if (result != ResultCode::OK) return result;

        fields.sinks.emplace_back(sink);
      }
    }
    if (json["sources"].is<JsonVariantConst>()) {
      for (JsonVariantConst sourceJson : json["sources"].as<JsonArrayConst>()) {
        TypedJson taggedJson;
        result = taggedJson.fromJson(sourceJson);

        if (result != ResultCode::OK) return result;

        SourceConfigVariant source;
        result = SourceFactory(taggedJson, source);

        if (result != ResultCode::OK) return result;

        fields.sources.emplace_back(source);
      }
    }
    return ResultCode::OK;
  }

}