
#include "GpioPiGpio.h"

#include "DigitalInputsRequestImplPiGpio.h"
#include "GpioLinesImplPiGpio.h"
#include "io/control/device/gpio/GpioException.h"


GpioPiGpio::GpioPiGpio()
{
   m_initRc = gpioInitialise(); 
}

GpioPiGpio::~GpioPiGpio()
{
    GpioPiGpio::close();
    if (m_initRc >= 0) {
        gpioTerminate();
    }   
}
    
bool
GpioPiGpio::isPresent()
{
    return true;
}

bool
GpioPiGpio::open()
{
  return m_initRc;
}

bool
GpioPiGpio::close()
{
  return true;
}

DigitalInputLinesRequest*
GpioPiGpio::requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& lines)
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
GpioPiGpio::requestDigitalOutputs(const char * contextId, const std::vector<DigitalOutput*>& lines)
{
  throw GpioException("GpioImplPiGpio::requestDigitalOutputs not implemented yet");
}
