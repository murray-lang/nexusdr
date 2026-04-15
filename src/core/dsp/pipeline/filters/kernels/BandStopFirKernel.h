//
// Created by murray on 22/02/25.
//

#ifndef CUTESDR_VK6HL_BANDSTOPFIRKERNEL_H
#define CUTESDR_VK6HL_BANDSTOPFIRKERNEL_H

#include "FirKernel.h"

class BandStopFirKernel : public FirKernel {
public:
  BandStopFirKernel() = default;

  void configure(int32_t freqLoPass, int32_t freqHiPass, int32_t offset, uint32_t sampleRate)
  {
    const ComplexSamplesFft& complex = configureComplex(freqLoPass, freqHiPass, offset, sampleRate);
    complexToReal(complex, m_realCoefficients);
  }

protected:
  const ComplexSamplesFft& configureComplex(int32_t freqLoPass, int32_t freqHiPass, int32_t offset, uint32_t sampleRate);
  void clampCoefficientsToZero(ComplexSamplesFft& coefficients);

};


#endif //CUTESDR_VK6HL_BANDSTOPFIRKERNEL_H
