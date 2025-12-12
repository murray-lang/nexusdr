//
// Created by murray on 18/11/25.
//

#pragma once
#include "IqPipelineStage.h"
#include "dsp/iq/IqSink.h"
#include "dsp/iq/IqSource.h"
#include "io/audio/AudioSink.h"
#include "io/iq/IqIo.h"
#include "settings/Mode.h"
#include "../../io/control/PttSink.h"
#include <qcoreevent.h>


class ModeSettings;

class IqPipeline : public IqSink, public PttSink
{
public:
  explicit IqPipeline(const ModeSettings& modeSettings, QObject* eventTarget);
  ~IqPipeline() override = default;

  virtual void initialise(IqIo* pIo, AudioSink* pAudioOutSink)
  {
    m_pAudioOutSink = pAudioOutSink;
  }
  virtual void setOutputSampleRate(uint32_t sampleRate)
  {
    m_outputSampleRate = sampleRate;
  }

  [[nodiscard]] virtual uint32_t getMaxFramesPerInputPacket() const = 0;
  [[nodiscard]] virtual uint32_t getMaxFramesPerOutputPacket() const = 0;

  virtual void setMode(const Mode& mode)
  {
    m_mode = mode;
  }

protected:
  void addStage(IqPipelineStage* pStage)
  {
    m_stages.push_back(pStage);
  }

protected:
  const ModeSettings& m_modeSettings;
  QObject* m_eventTarget;
  Mode m_mode;
  std::mutex m_settingsMutex;
  // IqPipelineIo* m_pIo;
  // IqSource* m_pInput;
  std::vector<IqPipelineStage*> m_stages;
  ComplexPingPongBuffers m_buffers;
  uint32_t m_inputSampleRate;
  uint32_t m_outputSampleRate;
  AudioSink* m_pAudioOutSink;
};
