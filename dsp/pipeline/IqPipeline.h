//
// Created by murray on 18/11/25.
//

#pragma once
#include "IqPipelineStage.h"
#include "dsp/iq/IqSink.h"
#include "dsp/iq/IqSource.h"
#include "io/audio/AudioSink.h"
#include "io/iq/IqIo.h"
#include "config-settings/settings/Mode.h"
#include "../../io/control/PttSink.h"
#include "oscillators/OscillatorMixer.h"
#include <qcoreevent.h>

#include "config-settings/settings/RfSettings.h"


class PipelineSettings;
class ModeSettings;

class IqPipeline : public IqSink, public PttSink
{
public:
  explicit IqPipeline(QObject* eventTarget);
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
  virtual void calcNyquistOffsetsLimits(int32_t* maxNegative, int32_t* maxPositive) const = 0;

  virtual void setMode(const Mode& mode)
  {
    m_mode = mode;
  }

  bool adjustRfSettingsToLimits(RfSettings& rfSettings) const
  {
    int32_t maxNegative, maxPositive;
    calcNyquistOffsetsLimits(&maxNegative, &maxPositive);
    return rfSettings.applyMaximumVfoOffsets(maxNegative, maxPositive);
  }

  virtual void apply(const PipelineSettings* settings);

protected:
  void appendStage(IqPipelineStage* pStage)
  {
    m_stages.push_back(pStage);
  }

  void prependStage(IqPipelineStage* pStage)
  {
    m_stages.insert(m_stages.begin(), pStage);
  }

  void removeFirstStage()
  {
    m_stages.erase(m_stages.begin());
  }

protected:
  // const ModeSettings& m_modeSettings;
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
  OscillatorMixer m_oscillatorMixer;
};
