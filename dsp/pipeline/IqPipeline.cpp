//
// Created by murray on 18/11/25.
//
#include "IqPipeline.h"
#include "config-settings/settings/pipeline/PipelineSettings.h"

#define DEFAULT_PING_PONG_LENGTH 8192

class RfSettings;
class ModeSettings;

IqPipeline::IqPipeline(QObject* eventTarget) :
  // m_modeSettings(modeSettings),
  m_eventTarget(eventTarget),
  m_mode(),
  m_buffers(DEFAULT_PING_PONG_LENGTH),
  m_inputSampleRate(0),
  m_outputSampleRate(0),
  m_pAudioOutSink(nullptr)
{
}

void
IqPipeline::apply(const PipelineSettings* settings)
{
  if (settings != nullptr) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    if (settings->hasSettingChanged(PipelineSettings::RF)) {
      const RfSettings& rfSettings = settings->getRfSettings();
      if (rfSettings.hasSettingChanged(RfSettings::CENTER_FREQUENCY)
          || rfSettings.hasSettingChanged(RfSettings::VFO)) {
        int64_t centreFreq = rfSettings.getCentreFrequency();
        int64_t vfo = rfSettings.getVfo();
        auto offset = static_cast<int32_t>(centreFreq - vfo);
        m_oscillatorMixer.setFrequency(offset);
      }
    }
    if (settings->hasSettingChanged(PipelineSettings::MODE)) {
      setMode(settings->getMode());
    }
  }
}
