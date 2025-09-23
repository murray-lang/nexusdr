
#include "GpioImplPiGpio.h"

#include "GpioLinesImplPiGpio.h"
#include "control/device/gpio/GpioException.h"


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
GpioImplPiGpio::requestLines(
  const char * contextId,
  const std::vector<uint32_t>& lines,
  GpioLines::Direction direction,
  GpioLines::Bias bias,
  GpioLines::Edge edge
)
{
  if (m_initRc < 0) {
    throw GpioException("PiGPIO not initialised");
  }
  auto pLines = new GpioLinesImplPiGpio(contextId); 
  try {
    pLines->request(contextId, lines, direction, bias, edge);
  } catch (...) {
    delete pLines;
    throw;
  }
  return dynamic_cast<GpioLines*>(pLines);

}
