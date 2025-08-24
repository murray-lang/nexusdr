//
// Created by murray on 15/04/23.
//

#include "ControlException.h"

ControlException::ControlException(const std::string& what)
        : std::runtime_error(what) {
}
