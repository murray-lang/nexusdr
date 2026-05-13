#pragma once
#include "MeteringSink.h"
#include "receiver/IqReceiverMetering.h"

class MeteringSource{
public:
  MeteringSource(MeteringSink* pSink) : m_pSink(pSink) {}

  void notifyRxMetering(IqReceiverMetering& rxMetering) const
  {
    if (m_pSink != nullptr) {
      m_pSink->updateRxMetering(rxMetering);
    }
  }

protected:
  MeteringSink* m_pSink;
};
