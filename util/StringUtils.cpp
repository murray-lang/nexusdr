//
// Created by murray on 2025-08-24.
//

#include "StringUtils.h"

#include <algorithm>

std::vector<std::string>
StringUtils::split(const std::string& s, char delim) {
  std::vector<std::string> parts;
  std::size_t start = 0;
  while (true) {
    std::size_t pos = s.find(delim, start);
    if (pos == std::string::npos) {
      parts.emplace_back(s.substr(start)); // last token (may be empty)
      break;
    }
    parts.emplace_back(s.substr(start, pos - start)); // may be empty if delim repeats
    start = pos + 1;
  }
  return parts;
}

std::string
StringUtils::toLowerCase(const std::string& str)
{
  std::string output = str;
  std::transform(output.begin(), output.end(), output.begin(),
                 [](unsigned char c){ return std::tolower(c); });
  return output;

}
