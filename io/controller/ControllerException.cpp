//
// Created by murray on 15/04/23.
//

#include "ControllerException.h"

ControllerException::ControllerException(const std::string& what)
        : std::runtime_error(what) {
}
