//
// Created by murray on 2025-08-21.
//

#ifndef CUTESDR_VK6HL_GPIO_H
#define CUTESDR_VK6HL_GPIO_H
#include <memory>

#include "digital/DigitalInputsRequest.h"

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

  DigitalInputsRequest* requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& inputs);

  class Impl
  {
  public:
    virtual ~Impl() = default;
    static bool isPresent();
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual DigitalInputsRequest* requestDigitalInputs(const char * contextId, const std::vector<DigitalInput*>& inputs) = 0;
  };

protected:
  std::unique_ptr<Impl> m_pImpl;

};




#endif //CUTESDR_VK6HL_GPIO_H