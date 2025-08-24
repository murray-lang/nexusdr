//
// Created by murray on 2025-08-24.
//

#ifndef CUTESDR_VK6HL_DIGITALINPUT_H
#define CUTESDR_VK6HL_DIGITALINPUT_H
#include <cstdint>
#include <vector>


class DigitalInput
{
public:
  DigitalInput();
  ~DigitalInput();

protected:
  std::vector<uint32_t> m_lines;
  std::vector<uint32_t> m_settingPath;
};


#endif //CUTESDR_VK6HL_DIGITALINPUT_H