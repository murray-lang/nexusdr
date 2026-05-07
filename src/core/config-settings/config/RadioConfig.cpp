#include "RadioConfig.h"

namespace Config::Radio
{
ResultCode fromJson(JsonVariantConst json, Fields& fields)
{
  ResultCode result = ResultCode::OK;
  if (json[Control::type].is<JsonVariantConst>()) {
    // TypedJson controlJson;
    // result = controlJson.fromJson(json[Control::type]);
    //
    // if (result == ResultCode::OK) {
      result = Control::fromJson(json[Control::type], fields.control);
    // }
  }

  if (result != ResultCode::OK) return result;

  if (json[Receiver::type].is<JsonVariantConst>()) {
    fields.receiver.emplace();
    result = Receiver::fromJson(json[Receiver::type], *fields.receiver);
  }

  if (result != ResultCode::OK) return result;

  if (json[Transmitter::type].is<JsonVariantConst>()) {
    fields.transmitter.emplace();
    result = Transmitter::fromJson(json[Transmitter::type], *fields.transmitter);
  }

  if (result != ResultCode::OK) return result;

  if (json[Ui::type].is<JsonVariantConst>()) {
    fields.ui.emplace();
    result = Ui::fromJson(json[Ui::type], *fields.ui);
  }
  return result;
}
}

// inline Config::Radio::Fields tempConfig {
//   .control{
//     .sinks = {
//       Config::Control::SinkConfigVariant {
//         Config::DigitalOutput::Fields {
//           "digitaloutput",
//           Config::GpioLines::Fields{
//             .lines{ 23 },
//             .direction = "output", // Shouldn't have to set this for outputs
//             .bias = "none",
//             .edge = "rising" // What about level?
//           },
//           "ptt" // settingPath
//         }
//       },
//       Config::Control::SinkConfigVariant{
//         Config::FunCube::Fields{ "funcube" }
//       }
//     },
//     // .sources = {
//     // }
//   },
//   .receiver{
//     .iqIo{
//     }
//   },
//   .transmitter{
//     .iqIo{
//     }
//   },
//   .ui{
//     .face = "default"
//   }
// };