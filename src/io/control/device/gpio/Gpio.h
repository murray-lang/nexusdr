#pragma once

#ifdef USE_GPIOD
#include "impl/gpiod/GpioGpiod.h"
#elif defined( USE_PIGPIO)
#include "impl/pigpio/GpioPiGpio.h"
#endif
