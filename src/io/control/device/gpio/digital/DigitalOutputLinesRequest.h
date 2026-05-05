#pragma once

#ifdef USE_GPIOD
#include "../impl/gpiod/DigitalOutputLinesRequestGpiod.h"
#endif
// #include <cstdint>
//
// #include "ResultCode.h"
// #include "etl/vector.h"
// // #include "DigitalOutputTypes.h"
// // #include "io/control/device/gpio/Gpio.h"
//
// #ifdef USE_ETL
// #include "etl/vector.h"
//
// using DigitalOutputLinesVector = etl::vector<uint32_t, MAX_GPIO_LINES_PER_DEVICE>;
// using DigitalOutputValuesVector = etl::vector<bool, MAX_GPIO_LINES_PER_DEVICE>;
//
// #else
// #include <vector>
//
// using DigitalOutputLinesVector = std::vector<uint32_t>;
// using DigitalOutputValuesVector = std::vector<bool>;
// #endif
//
// class Gpio;
// class DigitalOutput;
// class DigitalOutputVector;
//
// class DigitalOutputLinesRequest
// {
// public:
//   DigitalOutputLinesRequest();
//   virtual ~DigitalOutputLinesRequest() = default;
//
//   virtual ResultCode request(const char * contextId, const DigitalOutputVector& outputs) = 0;
//
//   virtual void release() = 0;
//   virtual int setLineValue(uint32_t line, bool value) = 0;
//   virtual int setLineValues(const DigitalOutputLinesVector& lines, const DigitalOutputValuesVector& values) = 0;
//
// protected:
//   Gpio& m_gpio;
// };

