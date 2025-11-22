//
// Created by murray on 18/11/25.
//
#include "IqPipeline.h"

#define DEFAULT_PING_PONG_LENGTH 8192

IqPipeline::IqPipeline(QObject* eventTarget) :
  m_eventTarget(eventTarget),
  m_mode(),
  m_buffers(DEFAULT_PING_PONG_LENGTH),
  m_inputSampleRate(0),
  m_outputSampleRate(0),
  m_pAudioOutSink(nullptr)
{
}
