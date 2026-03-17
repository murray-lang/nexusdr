//
// Created by murray on 29/9/25.
//

#pragma once

#include <cstdint>

#include "core/config-settings/config/GpioLinesConfig.h"


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
  explicit GpioLines(const std::vector<uint32_t>& lines);
  explicit GpioLines(const GpioLinesConfig* pConfig);
  GpioLines(const std::vector<uint32_t>& lines, const GpioLinesConfig* pConfig);
  GpioLines(const std::vector<uint32_t>& lines, Direction direction, Bias bias, Edge edge);
  GpioLines(const GpioLines& other) = default;
  virtual ~GpioLines() = default;

  GpioLines& operator=(const GpioLines& other);

  [[nodiscard]] const std::vector<uint32_t>& getLines() const { return m_lines; }
  [[nodiscard]] Direction getDirection() const { return m_direction; }
  [[nodiscard]] Bias getBias() const { return m_bias; }
  [[nodiscard]] Edge getEdge() const { return m_edge; }

  void setLineNo(std::vector<uint32_t> lines) { m_lines = lines; }
  void setDirection(Direction direction) { m_direction = direction; }
  void setBias(Bias bias) { m_bias = bias; }
  void setEdge(Edge edge) { m_edge = edge; }

  void configure(const GpioLinesConfig* pConfig);

protected:
  std::vector<uint32_t> m_lines;
  Direction m_direction;
  Bias m_bias;
  Edge m_edge;

};
