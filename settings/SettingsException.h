//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_SETTINGSEXCEPTION_H
#define CUTESDR_VK6HL_SETTINGSEXCEPTION_H
#include <stdexcept>

class SettingsException : public std::runtime_error
{
public:
  explicit SettingsException(const std::string& what) : std::runtime_error(what) {};
};


#endif //CUTESDR_VK6HL_SETTINGSEXCEPTION_H