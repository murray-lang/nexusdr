#pragma once

#ifdef USE_GPIOD
#include "../impl/gpiod/DigitalInputLinesRequestGpiod.h"
#elif defined( USE_PIGPIO)
#include "../impl/pigpio/DigitalInputLinesRequestPiGpio.h"
#endif
