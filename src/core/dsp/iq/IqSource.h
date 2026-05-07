//
// Created by murray on 17/11/25.
//

#pragma once
#include "ResultCode.h"
#include "IqSink.h"
#include "../../config-settings/config/base/ConfigBase.h"

class IqSource
{
public:
  IqSource() : m_pIqSink(nullptr) {}
  explicit IqSource(IqSink* iqSink) : m_pIqSink(iqSink) {};
  virtual ~IqSource() = default;

  // virtual ResultCode configure(const ConfigBase* pConfig) = 0;

  virtual ResultCode start(uint32_t maxPacketFrames) = 0;
  virtual void stop() = 0;

  void setIqSink(IqSink* pIqSink) { m_pIqSink = pIqSink; }

  [[nodiscard]] virtual uint32_t getSampleRate() const = 0;

protected:
  IqSink* m_pIqSink;
};
