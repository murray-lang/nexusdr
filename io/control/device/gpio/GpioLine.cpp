//
// Created by murray on 29/9/25.
//

#include "GpioLine.h"

#include "config/ConfigException.h"

GpioLine::GpioLine() :
  m_lineNo(0),
  m_direction(Direction::AS_IS),
  m_bias(Bias::AS_IS),
  m_edge(Edge::NONE)
{

}
GpioLine::GpioLine(uint32_t lineNo) :
  m_lineNo(lineNo),
  m_direction(Direction::AS_IS),
  m_bias(Bias::AS_IS),
  m_edge(Edge::NONE)
{

}
GpioLine::GpioLine(const GpioLineConfig* pConfig) : GpioLine()
{
  configure(pConfig);
}

GpioLine::GpioLine(uint32_t line, const GpioLineConfig* pConfig) : GpioLine()
{
  configure(pConfig);
  m_lineNo = line;
}

GpioLine::GpioLine(uint32_t line, Direction direction, Bias bias, Edge edge) :
  m_lineNo(line),
  m_direction(direction),
  m_bias(bias),
  m_edge(edge)
{

}

GpioLine&
GpioLine::operator=(const GpioLine& other)
{
  if (this != &other) {
    m_lineNo = other.m_lineNo;
    m_direction = other.m_direction;
    m_bias = other.m_bias;
    m_edge = other.m_edge;
  }
  return *this;
}

void
GpioLine::configure(const GpioLineConfig* pConfig)
{
  const std::string& dir = pConfig->getDirection();
  if (dir == "input") {
    m_direction = Direction::INPUT;
  } else if (dir == "output") {
    m_direction = Direction::OUTPUT;
  } else {
    m_direction = Direction::AS_IS;
  }
  const std::string& bias = pConfig->getBias();
  if (bias == "disabled") {
    m_bias = Bias::DISABLED;
  } else if (bias == "pull-up") {
    m_bias = Bias::PULL_UP;
  } else if (bias == "pull-down") {
    m_bias = Bias::PULL_DOWN;
  } else {
    m_bias = Bias::AS_IS;
  }
  const std::string& edge = pConfig->getEdge();
  if (edge == "none") {
    m_edge = Edge::NONE;
  } else if (edge == "rising") {
    m_edge = Edge::RISING;
  } else if (edge == "falling") {
    m_edge = Edge::FALLING;
  } else if (edge == "both") {
    m_edge = Edge::BOTH;
  } else {
    m_edge = Edge::NONE;
  }
  m_lineNo = pConfig->getLine();
}