//
// Created by murray on 2025-08-21.
//

#ifndef CUTESDR_VK6HL_GPIOEXCEPTION_H
#define CUTESDR_VK6HL_GPIOEXCEPTION_H
#include <stdexcept>

class GpioException : public std::runtime_error
{
public:
    explicit GpioException(const std::string& what);
};


#endif //CUTESDR_VK6HL_GPIOEXCEPTION_H