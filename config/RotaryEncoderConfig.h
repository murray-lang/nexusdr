//
// Created by murray on 15/9/25.
//

#ifndef CUTESDR_VK6HL_ROTARYENCODERCONFIG_H
#define CUTESDR_VK6HL_ROTARYENCODERCONFIG_H
#include "DigitalInputConfig.h"

class RotaryEncoderConfig : public DigitalInputConfig
{
public:
  static constexpr auto type = "rotaryencoder";

  RotaryEncoderConfig() : DigitalInputConfig(type) {}
  ~RotaryEncoderConfig() override = default;

  void initialise(const nlohmann::json& json) override
  {
    DigitalInputConfig::initialise(json);
  }
};
#endif //CUTESDR_VK6HL_ROTARYENCODERCONFIG_H