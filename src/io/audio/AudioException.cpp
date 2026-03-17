//
// Created by murray on 1/01/25.
//

#include "AudioException.h"

AudioException::AudioException(const std::string& what)
    : std::runtime_error(what) {
}
