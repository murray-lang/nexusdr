//
// Created by murray on 29/9/25.
//

#include "GpioLines.h"

GpioLines::GpioLines() :
  m_direction(Direction::AS_IS),
  m_bias(Bias::AS_IS),
  m_edge(Edge::NONE)
{

}

GpioLines::GpioLines(Direction direction) :
  m_direction(direction),
  m_bias(Bias::AS_IS),
  m_edge(Edge::NONE)
{
}

GpioLines::GpioLines(const std::vector<uint32_t>& lines) :
  m_lines(lines),
  m_direction(Direction::AS_IS),
  m_bias(Bias::AS_IS),
  m_edge(Edge::NONE)
{

}
GpioLines::GpioLines(const Config::GpioLines::Fields& config) : GpioLines()
{
  configureLines(config);
}

GpioLines::GpioLines(const std::vector<uint32_t>& lines, const Config::GpioLines::Fields& config) : GpioLines()
{
  configureLines(config);
  m_lines = lines;
}

GpioLines::GpioLines(const std::vector<uint32_t>& lines, Direction direction, Bias bias, Edge edge) :
  m_lines(lines),
  m_direction(direction),
  m_bias(bias),
  m_edge(edge)
{

}

GpioLines&
GpioLines::operator=(const GpioLines& other)
{
  if (this != &other) {
    m_lines = other.m_lines;
    m_direction = other.m_direction;
    m_bias = other.m_bias;
    m_edge = other.m_edge;
  }
  return *this;
}

ResultCode
GpioLines::configureLines(const Config::GpioLines::Fields& config)
{
  const std::string& dir = config.direction;
  if (dir == "input") {
    m_direction = Direction::INPUT;
  } else if (dir == "output") {
    m_direction = Direction::OUTPUT;
  } else {
    m_direction = Direction::AS_IS;
  }
  const std::string& bias = config.bias;
  if (bias == "disabled") {
    m_bias = Bias::DISABLED;
  } else if (bias == "pull-up") {
    m_bias = Bias::PULL_UP;
  } else if (bias == "pull-down") {
    m_bias = Bias::PULL_DOWN;
  } else {
    m_bias = Bias::AS_IS;
  }
  const std::string& edge = config.edge;
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
  m_lines = config.lines;
  return ResultCode::OK;
}