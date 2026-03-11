#include "Gpio.h"

#ifdef USE_GPIOD
#include "impl/gpiod/GpioImplGpiod.h"
#endif 

#ifdef USE_PIGPIO
#include "impl/pigpio/GpioImplPiGpio.h"
#endif  

Gpio::Gpio()
{
#ifdef USE_GPIOD
    m_pImpl = std::make_unique<GpioImplGpiod>();
#elif defined(USE_PIGPIO)
    m_pImpl = std::make_unique<GpioImplPiGpio>();    
#else
    m_pImpl = nullptr;
#endif
}

bool Gpio::isPresent()
{
#ifdef USE_GPIOD
    return GpioImplGpiod::isPresent();
#elif defined(USE_PIGPIO)
    return GpioImplPiGpio::isPresent();    
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

DigitalInputLinesRequest*
Gpio::requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& inputs)
{
  return m_pImpl->requestDigitalInputs(contextId, inputs);
}

DigitalOutputLinesRequest*
Gpio::requestDigitalOutputs(const char * contextId, const std::vector<DigitalOutput*>& inputs)
{
  return m_pImpl->requestDigitalOutputs(contextId, inputs);
}

