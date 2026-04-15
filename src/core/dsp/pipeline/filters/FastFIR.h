#pragma once

#include "../../../SampleTypes.h"
#include "../IqPipelineStage.h"
#include "../../utils/pocketfft/pocketfft_hdronly.h"
#include "../../utils/constants.h"
#include "kernels/BandPassFirKernel.h"
#include "kernels/BandStopFirKernel.h"
#include "kernels/LowPassFirKernel.h"
#include "OverlapBuffers.h"
#include <cmath>
#include <algorithm>
#include <functional>
#include <qdebug.h>


template<class kernel>
class FastFIR : public IqPipelineStage
{
public:
  explicit FastFIR() :
      m_kernel(),
      m_inputCentre(FFT_SIZE / 2),
      m_inputCursor(0),
      m_pocketfft_shape{FFT_SIZE},
      m_pocketfft_stride{sizeof(sdrcomplex)},
      m_pocketfft_axes{0}//,
//  m_overlapBuffers(FIR_SIZE(fftSize))
  {
    initialise();
    //setWindowFunction(m_window);
  }
  virtual ~FastFIR() = default;

  void initialise()
  {
    //sdrcomplex zero(0.0, 0.0);
    m_overlapBuffer.assign(FIR_SIZE, sdrcomplex(0, 0));
    m_inputBuffer.assign(FFT_SIZE, sdrcomplex(0, 0));
    m_outputBuffer.assign(FFT_SIZE, sdrcomplex(0, 0));
  }

  uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength ) override
  {
//    sampleBuffers.flip();
//    return inputLength;
    uint32_t outPos = 0;
    const ComplexSamplesMax& input = buffers.input();
    ComplexSamplesMax& output = buffers.output();
    for (uint32_t inputIndex = 0; inputIndex < inputLength; inputIndex++)
    {
//      const sampleType& nextInput = inputIndex < 10 ? 0.0 : samplesInput[inputIndex];
      const sdrcomplex& nextInput = input[inputIndex];
      m_overlapBuffer.at(m_inputCursor) = nextInput;
//      m_overlapBuffer.at(m_currentInputCursor) = inputIndex;
      uint32_t fftInputIndex = m_inputCentre + m_inputCursor++;
      m_inputBuffer.at(fftInputIndex) = nextInput;
//      m_inputBuffer.at(fftInputIndex) = inputIndex;
      if (m_inputCursor == FIR_SIZE - 1)
      {
//        std::copy(m_inputBuffer.begin(), m_inputBuffer.end(), m_outputBuffer.begin());
        applyFilter(m_inputBuffer, m_outputBuffer);
        for(uint32_t filteredIndex = m_inputCentre; filteredIndex < FFT_SIZE; filteredIndex++) // FFT-based
        {
          output.at(outPos++) = m_outputBuffer.at(filteredIndex);
        }
//        std::copy(m_outputBuffer.begin(), m_outputBuffer.begin() + m_firSize - 1, samplesOutput.begin() + outPos);
//        std::copy(m_outputBuffer.begin(), m_outputBuffer.end(), samplesOutput.begin() + outPos);
//        outPos += m_firSize - 1;
        for (uint32_t overlapIndex = 0; overlapIndex < FIR_SIZE - 1; overlapIndex++)
        {
          m_inputBuffer.at(overlapIndex) = m_overlapBuffer.at(overlapIndex);
        }
        m_inputCursor = 0;
      }
    }
    //qDebug() << outPos << ",";
    return outPos;
  }

//  uint32_t _processSamples(
//      PingPongBuffers<sampleType>& sampleBuffers,
//      uint32_t inputLength,
//      SignalEmitter* chartSignaller
//  )
//  {
//    uint32_t outPos = 0;
//    const std::vector<sampleType>& samplesInput = sampleBuffers.input();
//    std::vector<sampleType>& samplesOutput = sampleBuffers.output();
//    for (uint32_t inputIndex = 0; inputIndex < inputLength; inputIndex++)
//    {
//      const sampleType& nextInput = samplesInput[inputIndex]; // * static_cast<float>(hanning(inputIndex, m_fftSize));
//      OverlapBufferState state = m_overlapBuffers.addSample(nextInput);
//      if (state == eREADY) {
//        std::vector<sampleType>& nextBuffer = m_overlapBuffers.getOverlapped();
//        std::vector<sampleType> filtered(m_firSize*2);
//        applyFilter(nextBuffer, filtered, chartSignaller);
//        for (const sampleType& outSample : filtered)
//        {
//          samplesOutput.at(outPos++) = outSample;
//        }
//      }
//    }
//    return outPos;
//  }

  kernel& getKernel() { return m_kernel; }

private:
  void applyFilter(ComplexSamplesFft& input, ComplexSamplesFft& output)
  {
    applyFftCoefficients(input, output);
//    applyConvolution(input, output, chartSignaller);
//    std::copy(input.begin(), input.end(), output.begin());
  }

//  void applyFilter(PingPongBuffers<sdrreal>& buffers, SignalEmitter* chartSignaller)
//  {
//    applyConvolution(buffers, chartSignaller);
//  }

  void applyFilter(const ComplexSamplesFft& input, ComplexSamplesFft& output)
  {
//    applyConvolution(input, output, chartSignaller);
    applyFftCoefficients(input, output);
  }

  void applyConvolution(ComplexSamplesFft& input, ComplexSamplesFft& output)
  {
    static uint32_t count = 0;
//    std::copy(input.begin(), input.end(), output.begin());
    const ComplexSamplesFft& sincPulse = m_kernel.getComplexSincPulse();
    uint32_t inputSize = input.size();

    // Convolve input with kernel
    for (uint32_t i = 0; i <= inputSize - FIR_SIZE; ++i) {
      // Apply std::inner_product for the convolution
      output.at(i) = std::inner_product(
          input.begin() + i,                 // Start of the input sliding window
          input.begin() + i + FIR_SIZE,    // End of the input sliding window
          sincPulse.begin(),                    // Start of the kernel
          sdrcomplex(0.0, 0.0)                                // Initial value of the summation
      );
    }
    qDebug() << count++;
  }

  void applyConvolution(const RealSamplesFft& input, RealSamplesFft& output)
  {
    const RealSamplesFft& sincPulse = m_kernel.getRealSincPulse();
    uint32_t inputSize = input.size();

    // Convolve input with kernel
    for (uint32_t i = 0; i < inputSize; ++i) {
      // Apply std::inner_product for the convolution
      output.at(i) = std::inner_product(
          input.begin() + i,                 // Start of the input sliding window
          input.begin() + i + FIR_SIZE,    // End of the input sliding window
          sincPulse.begin(),                    // Start of the kernel
          0.0                                // Initial value of the summation
      );
    }

  }

  void applyFftCoefficients(const ComplexSamplesFft& input, ComplexSamplesFft& output)
  {
    uint32_t inputSize = input.size();
    // ComplexPingPongBuffers internalBuffers;
    ComplexSamplesFft localInput, localOutput;
//    const std::vector<sampleType>& signalInput = input;
//    std::vector<sampleType>& windowedInput = internalBuffers.input();
//    windowedInput.assign(windowedInput.size(), static_cast<sampleType>(0));

//    for (uint32_t i = 0; i < inputSize; i++) {
//      windowedInput.at(i) = signalInput.at(i);// * static_cast<float>(hanning(i, m_fftSize));
//    }
    localInput.resize(inputSize);
    localOutput.resize(inputSize);
    fft(input, localInput, pocketfft::FORWARD);


    multiplyByCoefficients(localInput, localOutput);

    fft(localOutput, output, pocketfft::BACKWARD);
  }

  void multiplyByCoefficients(const ComplexSamplesFft& values, ComplexSamplesFft& result)
  {
    const ComplexSamplesFft& coefficients = m_kernel.getComplexCoefficients();
    // const ComplexSamplesFft unity(FFT_SIZE, sdrcomplex(0.0, 0.0));
    std::transform(
        std::begin(values),
        std::end(values),
        std::begin(coefficients),
        std::begin(result),
        std::multiplies<>()
    );
  }

  void multiplyByCoefficients(const RealSamplesFft& values, RealSamplesFft& result)
  {
    std::transform(
        std::begin(values),
        std::end(values),
        std::begin(m_kernel.getRealCoefficients()),
        std::begin(result),
        std::multiplies<>()
    );
  }

  void fft(const ComplexSamplesFft& in, ComplexSamplesFft& out, bool isForward)
  {
    pocketfft::c2c(
        m_pocketfft_shape,
        m_pocketfft_stride,
        m_pocketfft_stride,
        m_pocketfft_axes,
        isForward,
        in.data(),
        out.data(),
        static_cast<sdrreal>(1.0)
        //isForward ? (static_cast<sdrreal>(1.0)/static_cast<sdrreal>(m_fftSize)) : static_cast<sdrreal>(1.0)
    );
//    if (chartSignaller != nullptr && isForward) {
//      chartSignaller->emitFft(out, out.size());
//    }

//    vsdrcomplex * pfft = new vsdrcomplex(out);
//    SharedComplexSeriesData sharedFft = SharedComplexSeriesData(pfft);
//    signalComplexFftAvailable(sharedFft);

  }

  void fft(const RealSamplesFft& in, RealSamplesFft& out, bool isForward)
  {
    pocketfft::r2r_fftpack(
        m_pocketfft_shape,
        m_pocketfft_stride,
        m_pocketfft_stride,
        m_pocketfft_axes,
        true,
        isForward,
        in.data(),
        out.data(),
        isForward ? (static_cast<sdrreal>(1.0)/static_cast<sdrreal>(FFT_SIZE)) : static_cast<sdrreal>(1.0)
    );
  }

private:
  kernel m_kernel;
  uint32_t m_inputCentre;
  uint32_t m_inputCursor;

//  PingPongBuffers<sampleType> m_fftBuffers;
  ComplexSamplesFft m_inputBuffer;
  ComplexSamplesFft m_outputBuffer;
  ComplexSamplesFir m_overlapBuffer;

  pocketfft::shape_t m_pocketfft_shape;
  pocketfft::stride_t m_pocketfft_stride;
  pocketfft::shape_t m_pocketfft_axes;

//  OverlapBuffers<sampleType> m_overlapBuffers;
};

using BandPassFilter = FastFIR<BandPassFirKernel>;
using BandStopFilter = FastFIR<BandStopFirKernel>;
using LowPassFilter = FastFIR<LowPassFirKernel>;
