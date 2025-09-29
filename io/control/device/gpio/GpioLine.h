//
// Created by murray on 29/9/25.
//

#ifndef CUTESDR_VK6HL_GPIOLINE_H
#define CUTESDR_VK6HL_GPIOLINE_H
#include <cstdint>

#include "config/GpioLineConfig.h"


class GpioLine
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

  GpioLine();
  explicit GpioLine(uint32_t lineNo);
  explicit GpioLine(const GpioLineConfig* pConfig);
  GpioLine(uint32_t line, const GpioLineConfig* pConfig);
  GpioLine(uint32_t line, Direction direction, Bias bias, Edge edge);
  GpioLine(const GpioLine& other) = default;
  virtual ~GpioLine() = default;

  GpioLine& operator=(const GpioLine& other);

  [[nodiscard]] uint32_t getLineNo() const { return m_lineNo; }
  [[nodiscard]] Direction getDirection() const { return m_direction; }
  [[nodiscard]] Bias getBias() const { return m_bias; }
  [[nodiscard]] Edge getEdge() const { return m_edge; }

  void setLineNo(uint32_t lineNo) { m_lineNo = lineNo; }
  void setDirection(Direction direction) { m_direction = direction; }
  void setBias(Bias bias) { m_bias = bias; }
  void setEdge(Edge edge) { m_edge = edge; }

  void configure(const GpioLineConfig* pConfig);

protected:
  uint32_t m_lineNo;
  Direction m_direction;
  Bias m_bias;
  Edge m_edge;

};


#endif //CUTESDR_VK6HL_GPIOLINE_H