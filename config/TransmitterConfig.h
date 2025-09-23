//
// Created by murray on 27/07/25.
//

#ifndef CUTESDR_VK6HL_TRANSMITTERCONFIG_H
#define CUTESDR_VK6HL_TRANSMITTERCONFIG_H
#include "ConfigBase.h"

class TransmitterConfig : public ConfigBase
{
  friend class RadioConfig;
public:
  static constexpr auto type = "transmitter";
  explicit TransmitterConfig() : ConfigBase(type) {}
  ~TransmitterConfig() override = default;

  // TransmitterConfig(const TransmitterConfig& rhs)
  // {
  //   operator=(rhs);
  // }
  //
  // TransmitterConfig& operator=(const TransmitterConfig& rhs)
  // {
  //   return *this;
  // }

  void initialise(const nlohmann::json& json) override
  {
  }

protected:
};
#endif //CUTESDR_VK6HL_TRANSMITTERCONFIG_H
