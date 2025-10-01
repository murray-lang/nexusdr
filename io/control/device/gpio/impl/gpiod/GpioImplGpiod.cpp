
#include "GpioImplGpiod.h"

#include "GpioLinesRequestImplGpiod.h"
#include "io/control/device/gpio/GpioException.h"


GpioImplGpiod::GpioImplGpiod() :
    m_pChip()
{
    
}

GpioImplGpiod::~GpioImplGpiod()
{
  GpioImplGpiod::close();
}
    
bool
GpioImplGpiod::isPresent()
{
    gpiod_chip* pChip = gpiod_chip_open(defaultChipPath);
    if (pChip == nullptr) {
        return false;
    }
    gpiod_chip_close(pChip);
    return true;
}
bool
GpioImplGpiod::open()
{
  m_pChip = gpiod_chip_open(defaultChipPath);
  if (m_pChip == nullptr) {
    std::cerr << "GpioImplGpiod::open: Failed to open GPIO chip at " << defaultChipPath << std::endl;
    return false;
  }
  return true;
}

bool
GpioImplGpiod::close()
{
  if (m_pChip) {
    gpiod_chip_close(m_pChip);
    m_pChip = nullptr;
    return true;
  }
  return false;
}

GpioLinesRequest*
GpioImplGpiod::requestLines(const char * contextId, const std::vector<GpioLines>& lines)
{
  if (!m_pChip) {
    throw GpioException("GPIO chip not opened");
  }
  auto pLines = new GpioLinesRequestImplGpiod(m_pChip, contextId);
  try {
    pLines->request(contextId, lines);
  } catch (...) {
    delete pLines;
    throw;
  }
  return dynamic_cast<GpioLinesRequest*>(pLines);

}
