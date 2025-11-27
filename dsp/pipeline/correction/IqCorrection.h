//
// Created by murray on 27/11/25.
//

#pragma once
#include "dsp/pipeline/IqPipelineStage.h"

class IqCorrection : public IqPipelineStage
{
  public:
  IqCorrection() :
    m_amplitudeCorrection(0.0),
    m_phaseCorrection(0.0)
  {

  }
  ~IqCorrection() override = default;

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength) override
  {
    // Calculate correction coefficients once per block for efficiency.
    // m_amplitudeCorrection is treated as an additive offset (e.g., 0.0 = unity gain).
    const sdrreal gain = static_cast<sdrreal>(1.0) + m_amplitudeCorrection;

    // Use std::polar to compute phase components as requested.
    // This provides the sin/cos values needed to de-skew the I/Q axes.
    // We create a unit vector with angle 'm_phaseCorrection'.
    const std::complex<sdrreal> phasePhasor = std::polar(static_cast<sdrreal>(1.0), m_phaseCorrection);
    const sdrreal cosPhi = phasePhasor.real();
    const sdrreal sinPhi = phasePhasor.imag();

    // Assuming ComplexPingPongBuffers provides access to the current working buffer.
    // We iterate through the samples to apply the affine transform.
    // Note: Adjust the buffer access (.exchange, .buffer, or operator[]) according to your specific class definition.
    auto& input = buffers.input();
    auto& output = buffers.output();

    for (uint32_t i = 0; i < inputLength; ++i)
    {
      sdrcomplex& sample = input.at(i);

      // Apply IQ Imbalance Correction
      // 1. Remove the part of I that "leaked" into Q due to non-orthogonality (Phase Correction).
      //    Formula: Q_new = (Q_old - I * sin(phi)) / cos(phi)
      // 2. Normalize the gain of Q to match I (Amplitude Correction).

      sdrreal iVal = sample.real();
      sdrreal qVal = sample.imag();

      // Protect against divide by zero if phase is exactly 90 degrees (unlikely in practice)
      sdrreal qCorrected = (std::abs(cosPhi) > 1e-6) ? (qVal - iVal * sinPhi) / cosPhi : qVal;

      // Apply amplitude balance to Q
      qCorrected *= gain;

      output.at(i) = { iVal, qCorrected };
    }
    return inputLength;
  }
protected:
  sdrreal m_amplitudeCorrection;
  sdrreal m_phaseCorrection;

};