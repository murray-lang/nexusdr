#include "Gpio.h"

#ifdef USE_GPIOD
#include "impl/gpiod/GpioImplGpiod.h"
#endif  

Gpio::Gpio()
{
#ifdef USE_GPIOD
    m_pImpl = std::make_unique<GpioImplGpiod>();
#else
    m_pImpl = nullptr;
#endif
}

bool Gpio::isPresent()
{
#ifdef USE_GPIOD
    return GpioImplGpiod::isPresent();
#else
    return false;
#endif
}

void Gpio::open()
{
    m_pImpl->open();
}
void Gpio::close()
{
    if (m_pImpl) {
        m_pImpl->close();
    }
}

GpioLines*
Gpio::requestLines(
  const char * contextId,
  const std::vector<uint32_t>& lines,
  GpioLines::Direction direction,
  GpioLines::Bias bias,
  GpioLines::Edge edge
)
{
  return m_pImpl->requestLines(contextId, lines, direction, bias, edge);
}

