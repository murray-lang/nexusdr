//
// Created by murray on 1/01/25.
//

#ifndef CUTESDR_VK6HL_AUDIOEXCEPTION_H
#define CUTESDR_VK6HL_AUDIOEXCEPTION_H

#include <stdexcept>

class AudioException : public std::runtime_error
{
public:
  explicit AudioException(const std::string& what);
};

#endif //CUTESDR_VK6HL_AUDIOEXCEPTION_H
