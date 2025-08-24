//
// Created by murray on 20/8/25.
//

#ifndef RPI_GPIOROTARYENCODER_H
#define RPI_GPIOROTARYENCODER_H
#include "../../../ControlSource.h"

class GpioRotaryEncoder : public ControlSource
{
public:
  GpioRotaryEncoder();
  ~GpioRotaryEncoder() override = default;

  void initialise(const nlohmann::json& json) override;
  bool discover() override;
  void open() override;
  void close() override;
  void exit() override;

protected:
  std::vector<uint32_t> m_gpioPins;
  SettingPath m_settingPath;
};


#endif //RPI_GPIOROTARYENCODER_H