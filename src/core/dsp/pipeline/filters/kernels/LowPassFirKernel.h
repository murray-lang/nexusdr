//
// Created by murray on 18/02/25.
//

#pragma once


#include "FirKernel.h"

class LowPassFirKernel : public FirKernel {
public:
  LowPassFirKernel() = default;

  void configure(int32_t freqHiCut, int32_t offset, uint32_t sampleRate)
  {
    const ComplexSamplesFft& complex = configureComplex(freqHiCut, offset, sampleRate);
    complexToReal(complex, m_realCoefficients);
  }

protected:
  const ComplexSamplesFft& configureComplex(int32_t freqHiCut, int32_t offset, uint32_t sampleRate);

};

