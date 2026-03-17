//
// Created by murray on 17/12/25.
//

#pragma once
#include <cstdint>

typedef unsigned long AudioFormat;
static const AudioFormat AUDIO_SINT8 = 0x1;    // 8-bit signed integer.
static const AudioFormat AUDIO_SINT16 = 0x2;   // 16-bit signed integer.
static const AudioFormat AUDIO_SINT24 = 0x4;   // 24-bit signed integer.
static const AudioFormat AUDIO_SINT32 = 0x8;   // 32-bit signed integer.
static const AudioFormat AUDIO_FLOAT32 = 0x10; // Normalized between plus/minus 1.0.
static const AudioFormat AUDIO_FLOAT64 = 0x20; // Normalized between plus/minus 1.0.

class AudioDriver
{
protected:
  virtual ~AudioDriver() = default;

public:
  struct Format
  {
    uint32_t sampleRate;
    uint32_t channelCount;
    uint32_t bytesPerFrame;
    AudioFormat sampleFormat;
  };

  explicit AudioDriver(const Format& format) :
    m_format(format)
  {
  }

  virtual void start(uint32_t maxPacketFrames) = 0;
  virtual void stop() = 0;

  [[nodiscard]] uint32_t getSampleRate() const { return m_format.sampleRate; }

protected:
  Format m_format;
};