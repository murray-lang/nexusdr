//
// Created by murray on 18/02/25.
//

#pragma once


#include "FirKernel.h"

class BandPassFirKernel : public FirKernel
{
public:
  BandPassFirKernel() = default;

  void configure(int32_t freqLoCut, int32_t freqHiCut, int32_t offset, uint32_t sampleRate)
  {
    const ComplexSamplesFft& complex = configureComplex(freqLoCut, freqHiCut, offset, sampleRate);
    complexToReal(complex, m_realCoefficients);
  }

protected:
  const ComplexSamplesFft& configureComplex(int32_t freqLoCut, int32_t freqHiCut, int32_t offset, uint32_t sampleRate);


};
