
#include "GpioImplPiGpio.h"

#include "DigitalInputsRequestImplPiGpio.h"
#include "GpioLinesImplPiGpio.h"
#include "io/control/device/gpio/GpioException.h"


GpioImplPiGpio::GpioImplPiGpio()
{
   m_initRc = gpioInitialise(); 
}

GpioImplPiGpio::~GpioImplPiGpio()
{
    GpioImplPiGpio::close();
    if (m_initRc >= 0) {
        gpioTerminate();
    }   
}
    
bool
GpioImplPiGpio::isPresent()
{
    return true;
}

bool
GpioImplPiGpio::open()
{
  return m_initRc;
}

bool
GpioImplPiGpio::close()
{
  return true;
}

DigitalInputLinesRequest*
GpioImplPiGpio::requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& lines)
{
  if (m_initRc < 0) {
    throw GpioException("PiGPIO not initialised");
  }
  auto pLines = new DigitalInputsRequestImplPiGpio(contextId);
  try {
    pLines->request(contextId, lines);
  } catch (...) {
    delete pLines;
    throw;
  }
  return dynamic_cast<DigitalInputLinesRequest*>(pLines);

}

DigitalOutputLinesRequest*
GpioImplPiGpio::requestDigitalOutputs(const char * contextId, const std::vector<DigitalOutput*>& lines)
{
  throw GpioException("GpioImplPiGpio::requestDigitalOutputs not implemented yet");
}
