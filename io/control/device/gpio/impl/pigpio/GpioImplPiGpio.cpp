
#include "GpioImplPiGpio.h"

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

GpioLines*
GpioImplPiGpio::requestLines(const char * contextId, const std::vector<GpioLine>& lines)
{
  if (m_initRc < 0) {
    throw GpioException("PiGPIO not initialised");
  }
  auto pLines = new GpioLinesImplPiGpio(contextId); 
  try {
    pLines->request(contextId, lines);
  } catch (...) {
    delete pLines;
    throw;
  }
  return dynamic_cast<GpioLines*>(pLines);

}
