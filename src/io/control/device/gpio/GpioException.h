//
// Created by murray on 2025-08-21.
//

#pragma once

#include <stdexcept>

class GpioException : public std::runtime_error
{
public:
    explicit GpioException(const std::string& what);
};
