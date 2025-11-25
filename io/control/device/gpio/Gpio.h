//
// Created by murray on 2025-08-21.
//

#pragma once

#include <memory>

#include "digital/DigitalInputLinesRequest.h"
#include "digital/DigitalOutputLinesRequest.h"

class Gpio {
public:
  Gpio();
  static Gpio& getInstance() {
    static Gpio instance; // Only created once, thread-safe since C++11
    return instance;
  }

  // Delete copy and move operations
  Gpio(const Gpio&) = delete;
  Gpio& operator=(const Gpio&) = delete;
  Gpio(Gpio&&) = delete;
  Gpio& operator=(Gpio&&) = delete;

  static bool isPresent();
  void open();
  void close();

  DigitalInputLinesRequest* requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& inputs);
  DigitalOutputLinesRequest* requestDigitalOutputs(const char * contextId, const std::vector<DigitalOutput*>& inputs);

  class Impl
  {
  public:
    virtual ~Impl() = default;
    static bool isPresent();
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual DigitalInputLinesRequest* requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& inputs) = 0;
    virtual DigitalOutputLinesRequest* requestDigitalOutputs(const char * contextId, const std::vector<DigitalOutput*>& inputs) = 0;
  };

protected:
  std::unique_ptr<Impl> m_pImpl;

};
