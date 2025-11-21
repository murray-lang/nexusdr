//
// Created by murray on 2025-08-24.
//

#pragma once

#include <stdexcept>

class SettingsException : public std::runtime_error
{
public:
  explicit SettingsException(const std::string& what) : std::runtime_error(what) {};
};
