//
// Created by murray on 16/2/26.
//

#include "RxPipelineAudioTap.h"

uint32_t
RxPipelineAudioTap::sinkAudio(const RealSamplesMax& samples, uint32_t length, uint32_t numChannels)
{
  return m_router.sinkPipelineAudio(m_pipelineId, samples, length, numChannels);
}