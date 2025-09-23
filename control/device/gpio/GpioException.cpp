//
// Created by murray on 2025-08-21.
//

#include "GpioException.h"

GpioException::GpioException(const std::string& what)
        : std::runtime_error(what) {
}
