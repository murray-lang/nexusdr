#pragma once
#include "MonitorSink.h"

class MonitorSource
{
  public:
  MonitorSource(MonitorSink* pSink) : m_pSink(pSink) {}

  void notifyRxIq(const ComplexSamplesMax& iq, uint32_t len, uint32_t sampleRate ) const
  {
    if (m_pSink) {
      m_pSink->updateRxIq(iq, len, sampleRate);
    }
  }

  void notifyTxIq(const ComplexSamplesMax& iq, uint32_t len, uint32_t sampleRate ) const
  {
    if (m_pSink) {
      m_pSink->updateTxIq(iq, len, sampleRate);
    }
  }

  void notifyRxAudio(const RealSamplesMax& audio, uint32_t len, uint32_t sampleRate ) const
  {
    if (m_pSink) {
      m_pSink->updateRxAudio(audio, len, sampleRate);
    }
  }

protected:
  MonitorSink* m_pSink;
};
