//
// Created by murray on 18/9/25.
//

#pragma once

#include <vector>

#include "GpioPiGpio.h"
#include "../../digital/DigitalInputsRequest.h"


class DigitalInputsRequestPiGpio : public DigitalInputLinesRequest
{
  friend GpioPiGpio;
public:
  explicit DigitalInputsRequestPiGpio(const char* consumer = "");
  ~DigitalInputsRequestPiGpio() override;

  void startCallbacks(Callback* callback) override;
  void stopCallbacks() override;

  void request(const char * contextId, const std::vector<DigitalInput*>& lines) override;

  void release() override;

  // int debounce(LineStateMap& changes) override { return 0;};

  int getLineValue(uint32_t line) override;

protected:

  void setLineDirection(uint32_t line, GpioLines::Direction direction);
  void setLineBias(uint32_t line, GpioLines::Bias bias);
  static void gpioCallback(int gpio, int level, uint32_t tick, void* userData);


  
protected:
  Callback* m_pCallback;
  std::string m_consumer;

};
