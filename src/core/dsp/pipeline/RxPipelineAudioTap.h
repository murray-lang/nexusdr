//
// Created by murray on 16/2/26.
//

// radio/receiver/RxPipelineAudioTap.h
#pragma once

#include "io/audio/AudioSink.h"
#include "core/config-settings/settings/pipeline/PipelineId.h"

class RxPipelineAudioRouter
{
public:
  virtual ~RxPipelineAudioRouter() = default;
  virtual uint32_t sinkPipelineAudio(PipelineId pipelineId,
                                     const RealSamplesMax& samples,
                                     uint32_t length,
                                     uint32_t numChannels) = 0;
};

class RxPipelineAudioTap final : public AudioSink
{
public:
  RxPipelineAudioTap(RxPipelineAudioRouter& router, PipelineId pipelineId)
    : m_router(router), m_pipelineId(pipelineId) {}

  uint32_t sinkAudio(const RealSamplesMax& samples, uint32_t length, uint32_t numChannels) override;

private:
  RxPipelineAudioRouter& m_router;
  PipelineId m_pipelineId;
};