//
// Created by murray on 13/11/25.
//

#pragma once

#include <deque>

#include "../../SampleTypes.h"
#include <liquid/liquid.h>
#include <qdebug.h>


class HilbertTransform
{
public:
  explicit HilbertTransform(int numTaps, sdrreal upperBandFraction = 0.95)
  {
    initialise(numTaps, upperBandFraction); 
  }

  ~HilbertTransform()
  {
    clearState();
  }

  void initialise(int numTaps, sdrreal stopBandAttenuation = 100.0)
  {
    clearState();
    // Liquid's firhilbf_create(m, as)
    // m is the filter semi-length (total taps ~ 4m)
    // as is the stop-band attenuation in dB
    unsigned int m = static_cast<unsigned int>(numTaps) / 4;
    if (m < 1) {
      m = 1;
    }
    m_hilbertState = firhilbf_create(m, stopBandAttenuation);
  }
  

  uint32_t transform(const vsdrreal& input, uint32_t numSamples, uint32_t numChannels, vsdrcomplex& output)
  {
    // qDebug() << "HilbertTransform::transform(): numSamples =" << numSamples << ", numChannels =" << numChannels;
    auto N = static_cast<size_t>(numSamples/numChannels);

    for (size_t n = 0; n < N; ++n) {
      size_t i = n * numChannels;

      std::complex<float> y;
      firhilbf_r2c_execute(m_hilbertState, static_cast<float>(input.at(i)), &y);

      output.at(i) = y;
    }
    return N;
  }

protected:

  void clearState()
  {
    if (m_hilbertState) {
      firhilbf_destroy(m_hilbertState);
      m_hilbertState = nullptr;
    }
  }

protected:
  firhilbf m_hilbertState;
};
