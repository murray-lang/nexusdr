//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_STRINGUTILS_H
#define CUTESDR_VK6HL_STRINGUTILS_H
#include <string>
#include <vector>

class StringUtils
{
public:
  static std::vector<std::string> split(const std::string& s, char delim);
  static std::string toLowerCase(const std::string& str);
};


#endif //CUTESDR_VK6HL_STRINGUTILS_H