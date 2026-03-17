//
// Created by murray on 2025-08-24.
//

#pragma once

#include <string>
#include <vector>

class StringUtils
{
public:
  static std::vector<std::string> split(const std::string& s, char delim);
  static std::string toLowerCase(const std::string& str);
};
