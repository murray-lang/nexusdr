#include "RadioConfig.h"

namespace Config::Radio
{
Result fromJson(JsonVariantConst json, Fields& fields)
{
  Result result = Result::OK;
  if (json[Control::type]) {
    TypedJson controlJson;
    result = controlJson.fromJson(json[Control::type]);

    if (result == Result::OK) {
      result = Control::fromJson(controlJson, fields.control);
    }
  }

  if (result != Result::OK) return result;

  if (json[Receiver::type]) {
    result = Receiver::fromJson(json[Receiver::type], fields.receiver);
  }

  if (result != Result::OK) return result;

  if (json[Transmitter::type]) {
    result = Transmitter::fromJson(json[Transmitter::type], fields.transmitter);
  }

  if (result != Result::OK) return result;

  if (json[Ui::type]) {
    result = Ui::fromJson(json[Transmitter::type], fields.ui);
  }
  return result;
}
}

inline Config::Radio::Fields tempConfig {
  .control{
    .sinks = {
      Config::Control::SinkVariant {
        Config::DigitalOutput::Fields {
          "digitaloutput",
          Config::GpioLines::Fields{
            .lines{ 23 },
            .direction = "output", // Shouldn't have to set this for outputs
            .bias = "none",
            .edge = "rising" // What about level?
          },
          "ptt" // settingPath
        }
      },
      Config::Control::SinkVariant{
        Config::FunCube::Fields{ "funcube" }
      }
    },
    // .sources = {
    // }
  },
  .receiver{
    .iqIo{
    }
  },
  .transmitter{
    .iqIo{
    }
  },
  .ui{
    .face = "default"
  }
};