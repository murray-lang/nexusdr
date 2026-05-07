
#include "GpioGpiod.h"

#include "DigitalInputLinesRequestGpiod.h"
#include "DigitalOutputLinesRequestGpiod.h"

Gpio::Gpio() :
    m_pChip()
{
    
}

Gpio::~Gpio()
{
  close();
}
    
bool
Gpio::isPresent()
{
    gpiod_chip* pChip = gpiod_chip_open(defaultChipPath);
    if (pChip == nullptr) {
        return false;
    }
    gpiod_chip_close(pChip);
    return true;
}
ResultCode
Gpio::open()
{
  m_pChip = gpiod_chip_open(defaultChipPath);
  if (m_pChip == nullptr) {
   return ResultCode::ERR_GPIO_CHIP_OPEN;;
  }
  return ResultCode::OK;
}

void
Gpio::close()
{
  if (m_pChip) {
    gpiod_chip_close(m_pChip);
    m_pChip = nullptr;
  }
}

ResultCode
Gpio::requestDigitalInputs(
  const char * contextId,
  const DigitalInputVariantVector& inputs,
  DigitalInputLinesRequest& dilr
)
{
  if (!m_pChip) {
    return ResultCode::ERR_GPIOD_CHIP_NOT_OPENED;
  }
  dilr.initialise(m_pChip, contextId);
  return dilr.request(contextId, inputs);
}

ResultCode
Gpio::requestDigitalOutputs(
  const char * contextId,
  const DigitalOutputVariantVector& outputs,
  DigitalOutputLinesRequest& dolr
)
{
  if (!m_pChip) {
    return ResultCode::ERR_GPIOD_CHIP_NOT_OPENED;
  }
  dolr.initialise(m_pChip, contextId);
  return dolr.request(contextId, outputs);
}
