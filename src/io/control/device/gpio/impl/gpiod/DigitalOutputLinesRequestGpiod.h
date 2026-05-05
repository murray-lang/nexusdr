#pragma once
#include "ResultCode.h"
#include "GpioGpiod.h"

#ifdef USE_ETL
#include "etl/vector.h"

using DigitalOutputLinesVector = etl::vector<uint32_t, MAX_GPIO_LINES_PER_DEVICE>;
using DigitalOutputValuesVector = etl::vector<bool, MAX_GPIO_LINES_PER_DEVICE>;

#else
#include <vector>

using DigitalOutputLinesVector = std::vector<uint32_t>;
using DigitalOutputValuesVector = std::vector<bool>;
#endif


class DigitalOutputLinesRequest
{
public:
  explicit DigitalOutputLinesRequest();
  ~DigitalOutputLinesRequest();

  void initialise(gpiod_chip* pChip, const char* consumer = "");

  ResultCode request(const char * contextId, const DigitalOutputVariantVector& outputs);
  // ResultCode request(const char * contextId, const DigitalOutputVariant& output);

  void release();
  ResultCode setLineValue(uint32_t line, bool value);
  ResultCode setLineValues(const DigitalOutputLinesVector& lines, const DigitalOutputValuesVector& values);

protected:
  Gpio& m_gpio;
  gpiod_chip* m_pChip;
  std::string m_consumer;
  gpiod_line_request *m_pLineRequest;
};

