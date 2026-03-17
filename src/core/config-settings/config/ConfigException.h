//
// Created by murray on 27/07/25.
//

#pragma once

#include <stdexcept>

class ConfigException : public std::runtime_error
{
public:
  explicit ConfigException(const std::string& what) : std::runtime_error(what) {}
};
