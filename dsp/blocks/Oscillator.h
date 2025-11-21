#pragma once

#include <cstdint>
#include "../../SampleTypes.h"

class Oscillator
{
public:
  Oscillator();
  Oscillator(uint32_t sampleRate, int32_t frequency);

  Oscillator& initialise(uint32_t sampleRate, int32_t frequency);
  Oscillator& setSampleRate(int32_t sampleRate);
  Oscillator& setFrequency(int32_t frequency);
  [[nodiscard]] int32_t     getFrequency() const { return m_frequency; }
  Oscillator& increment();
  Oscillator& reset();

  Oscillator& operator++() { return increment(); }
  explicit operator const sdrcomplex& () const  { return m_state; }
  sdrcomplex operator * (const sdrcomplex& rhs) const { return m_state * rhs; }
//    complex operator * (const complex& rhs) const {
//        return complex(
//            rhs.real() * m_state.real() - rhs.imag() * m_state.imag(),
//            rhs.real() * m_state.imag() + rhs.imag() * m_state.real()
//        );
//    }
    sdrcomplex operator + (const sdrcomplex& rhs) const { return m_state + rhs; }
    sdrcomplex operator - (const sdrcomplex& rhs) const { return m_state - rhs; }

    sdrcomplex operator + (sdrreal rhs) const { return m_state + rhs; }
    sdrcomplex operator - (sdrreal rhs) const { return m_state - rhs; }
//    Oscillator& operator *= (const complex& rhs) {
//        m_state *= rhs;
//        return *this;
//    }
//    Oscillator& operator += (const complex& rhs) {
//        m_state += rhs;
//        return *this;
//    }
    //complex& multiply(const _complex& rhs, complex& result);
    //complex& add(const complex& rhs, complex& result);


protected:
    Oscillator& initialise();

protected:
  uint32_t m_sampleRate;
  int32_t m_frequency;
  sdrcomplex m_state;
  sdrreal m_theta;
  sdrreal m_thetaDelta;
  sdrreal m_sign;
};
