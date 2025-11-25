//
// Created by murray on 13/11/25.
//

#pragma once

#include <deque>

#include "SampleTypes.h"


class HilbertTransform
{
public:
  explicit HilbertTransform(int numTaps, sdrreal upperBandFraction = 0.95)
  {
    initialise(numTaps, upperBandFraction); 
  }

  void initialise(int numTaps, sdrreal upperBandFraction = 0.95)
  {
    m_coefficients = generateCoefficients(numTaps, upperBandFraction);
    m_delayLine.resize(m_coefficients.size(), 0.0);
  }
  

  uint32_t transform(const vsdrreal& input, vsdrcomplex& output, uint32_t numSamples)
  {
    auto N = static_cast<size_t>(numSamples); //input.size();
    size_t taps = m_coefficients.size();
    // output.resize(N);

    for (size_t n = 0; n < N; ++n) {
      // Shift in new sample
      m_delayLine.pop_back();
      m_delayLine.emplace_front(input.at(n));
      // Convolve for Hilbert (imag part)
      sdrreal imag = 0.0;
      for (size_t t = 0; t < taps; ++t) {
        imag += m_coefficients.at(t) * m_delayLine.at(t);
      }
      // Real is unchanged, imag is phase-shifted
      output[n] = sdrcomplex(input[n], imag);
    }
    return N;
  }

protected:

static vsdrreal generateCoefficients(int numTaps, sdrreal upperBandFraction)
  {
    vsdrreal result(numTaps, 0.0);
    int M = numTaps / 2;
    double fc = upperBandFraction * static_cast<double>(0.5);

    for (int n = 0; n < numTaps; ++n) {
      int k = n - M;
      if (k == 0) {
        result[n] = 0.0; // No DC
      } else if (k % 2 == 0) {
        result[n] = 0.0; // Zero @ even
      } else {
        result[n] = static_cast<sdrreal>((2.0 / (M_PI * static_cast<double>(k))) * std::sin(M_PI * fc * k));
      }
    }
    return result;
  }

protected:
  vsdrreal m_coefficients;
  std::deque<sdrreal> m_delayLine;
};
