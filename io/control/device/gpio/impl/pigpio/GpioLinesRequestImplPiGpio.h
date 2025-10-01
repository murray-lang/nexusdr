//
// Created by murray on 18/9/25.
//

#ifndef CUTESDR_VK6HL_GPIOLINESIMPLPIGPIO_H
#define CUTESDR_VK6HL_GPIOLINESIMPLPIGPIO_H
#include <vector>

#include "GpioImplPiGpio.h"
#include "../../GpioLinesRequest.h"


class GpioLinesRequestImplPiGpio : public GpioLines
{
  friend GpioImplPiGpio;
public:
  explicit GpioLinesRequestImplPiGpio(const char* consumer = "");
  ~GpioLinesRequestImplPiGpio() override;

  void startCallbacks(Callback* callback) override;
  void stopCallbacks() override;

  void request(const char * contextId, const std::vector<GpioLines>& lines) override;

  void release() override;

  int debounce(LineStateMap& changes) override { return 0;};

  int getLineValue(uint32_t line) override;

protected:

  void setLineDirection(uint32_t line, Direction direction);
  void setLineBias(uint32_t line, Bias bias);
  static void gpioCallback(int gpio, int level, uint32_t tick, void* userData);


  
protected:
  Callback* m_pCallback;
  std::string m_consumer;

};


#endif //CUTESDR_VK6HL_GPIOLINESIMPLPIGPIO_H