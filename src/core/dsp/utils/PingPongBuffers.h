#pragma once

#include "../../SampleTypes.h"
#include <etl/vector.h>
#include <cstdint>

template<typename SamplesType> class PingPongBuffers
{
public:
  explicit PingPongBuffers() : m_flip(false) {}

  SamplesType& input()
  {
    return m_flip ? m_pong : m_ping;
  }
  SamplesType& output() { return m_flip ? m_ping : m_pong; }

  SamplesType& current() { return m_flip ? m_pong : m_ping; }
  SamplesType& next() { return m_flip ? m_ping : m_pong; }

  PingPongBuffers& flip()
  {
    m_flip = !m_flip;
    return *this;
  }
  PingPongBuffers& reset() { m_flip = false; return *this; }

  PingPongBuffers& resize(uint32_t length) {
    m_ping.resize(length);
    m_pong.resize(length);
    return *this;
  }

  [[nodiscard]] uint32_t getSize() const { return m_ping.max_size(); }

private:
  SamplesType m_ping;
  SamplesType m_pong;
  bool m_flip;
};

using ComplexPingPongBuffers = PingPongBuffers<ComplexSamplesMax>;
using RealPingPongBuffers = PingPongBuffers<RealSamplesMax>;
