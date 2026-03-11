#include "DigitalOutputLinesRequest.h"
#include "../Gpio.h"

DigitalOutputLinesRequest::DigitalOutputLinesRequest() :
 m_gpio(Gpio::getInstance())
{}