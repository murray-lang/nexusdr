//
// Created by murray on 29/9/25.
//

#pragma once

#include <cstdint>

#include "core/config-settings/config/control/GpioLinesConfig.h"
#include "ResultCode.h"

#ifdef USE_ETL
#include <etl/vector.h>
using GpioLinesVector = etl::vector<uint32_t, MAX_GPIO_LINES_PER_DEVICE>;
#else
#include <vector>
using GpioLinesVector = std::vector<uint32_t>;
#endif


class GpioLines
{
public:

  enum class Direction {
    AS_IS = 1,
    INPUT,
    OUTPUT
  };

  enum class Bias {
    AS_IS = 1,
    UNKNOWN,
    DISABLED,
    PULL_UP,
    PULL_DOWN
  };

  enum class Edge {
    NONE = 1,
    RISING,
    FALLING,
    BOTH
  };

  GpioLines();
  explicit GpioLines(Direction direction);
  explicit GpioLines(const GpioLinesVector& lines);
  explicit GpioLines(const Config::GpioLines::Fields& config);
  GpioLines(const GpioLinesVector& lines, const Config::GpioLines::Fields& config);
  GpioLines(const GpioLinesVector& lines, Direction direction, Bias bias, Edge edge);
  // GpioLines(const GpioLines& other) = default;
  virtual ~GpioLines() = default;

  GpioLines(GpioLines&&)  noexcept = default;
  GpioLines& operator=(GpioLines&&)  noexcept = default;

  GpioLines& operator=(const GpioLines& other);

  [[nodiscard]] const GpioLinesVector& getLines() const { return m_lines; }
  [[nodiscard]] Direction getDirection() const { return m_direction; }
  [[nodiscard]] Bias getBias() const { return m_bias; }
  [[nodiscard]] Edge getEdge() const { return m_edge; }

  // void setLineNo(std::vector<uint32_t> lines) { m_lines = lines; }
  void setDirection(Direction direction) { m_direction = direction; }
  void setBias(Bias bias) { m_bias = bias; }
  void setEdge(Edge edge) { m_edge = edge; }

  ResultCode configureLines(const Config::GpioLines::Fields& config);

protected:
  GpioLinesVector m_lines;
  Direction m_direction;
  Bias m_bias;
  Edge m_edge;

};
