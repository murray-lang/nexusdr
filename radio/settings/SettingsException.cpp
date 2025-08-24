//
// Created by murray on 2025-08-24.
//

#include "SettingsException.h"

SettingsException::SettingsException(const std::string& what)
        : std::runtime_error(what) {
}