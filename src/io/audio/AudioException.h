//
// Created by murray on 1/01/25.
//

#pragma once


#include <stdexcept>

class AudioException : public std::runtime_error
{
public:
  explicit AudioException(const std::string& what);
};

