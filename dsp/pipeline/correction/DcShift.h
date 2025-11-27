#pragma once

#include "../IqPipelineStage.h"

class DcShift  : public IqPipelineStage
{
public:
    DcShift() : m_shift(0.0, 00) {}
    DcShift(const sdrcomplex& shift)  : m_shift(shift) {}

    virtual uint32_t processSamples(ComplexPingPongBuffers& buffers, uint32_t inputLength)
    {
        const vsdrcomplex& input = buffers.input();
        vsdrcomplex& output = buffers.output();

        for (uint32_t i = 0; i < inputLength; i++) {
            output[i] = input[i] + m_shift;
        }
        return inputLength;
    }

    DcShift& setShift(const sdrcomplex& shift) {
        m_shift = shift;
        return *this;
    }

    const sdrcomplex& getShift() const {
        return m_shift;
    }

protected:
    sdrcomplex m_shift;

};

