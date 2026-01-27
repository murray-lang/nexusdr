//
// Created by murray on 30/12/25.
//

#pragma once
#include <cstdint>
#include <string>

#include "../Mode.h"

class Band
{
public:
  Band() :
    m_lowestFrequency(0),
    m_highestFrequency(0),
    m_landingFrequency(0),
    m_defaultFineStep(0),
    m_defaultCoarseStep(0),
    m_defaultMode(Mode::Type::NONE)
  {}

  Band(
    const char* name,
    const char* label,
    int64_t lowestFrequency,
    int64_t highestFrequency,
    int64_t landingFrequency,
    int32_t defaultMinorStep,
    int32_t defaultMajorStep,
    Mode::Type defaultMode
  ) :
    m_name(name),
    m_label(label),
    m_lowestFrequency(lowestFrequency),
    m_highestFrequency(highestFrequency),
    m_landingFrequency(landingFrequency),
    m_defaultFineStep(defaultMinorStep),
    m_defaultCoarseStep(defaultMajorStep),
    m_defaultMode(defaultMode)
  {}
  Band(const Band& rhs) = default;
  ~Band() = default;

  Band& operator=(const Band& rhs) = default;
  // {
  //   if (this != &rhs) {
  //     m_name = rhs.m_name;
  //     m_label = rhs.m_label;
  //     m_lowestFrequency = rhs.m_lowestFrequency;
  //     m_highestFrequency = rhs.m_highestFrequency;
  //     m_landingFrequency = rhs.m_landingFrequency;
  //     m_defaultMode = rhs.m_defaultMode;
  //   }
  //   return *this;
  // }

  Band& invalidate()
  {
    m_name = "";
    m_label = "";
    m_lowestFrequency = 0;
    m_highestFrequency = 0;
    m_landingFrequency = 0;
    m_defaultFineStep = 0;
    m_defaultCoarseStep = 0;
    m_defaultMode = Mode::Type::NONE;
    return *this;
  }

  [[nodiscard]] bool isValid() const { return !m_name.empty(); }

  [[nodiscard]] const std::string& getName() const { return m_name; }
  [[nodiscard]] const std::string& getLabel() const { return m_label; }
  [[nodiscard]] int64_t getLowestFrequency() const { return m_lowestFrequency; }
  [[nodiscard]] int64_t getHighestFrequency() const { return m_highestFrequency; }
  [[nodiscard]] int64_t getLandingFrequency() const { return m_landingFrequency; }
  [[nodiscard]] int32_t getDefaultFineStep() const { return m_defaultFineStep; }
  [[nodiscard]] int32_t getDefaultCoarseStep() const { return m_defaultCoarseStep; }
  [[nodiscard]] Mode::Type getDefaultMode() const { return m_defaultMode; }

  bool containsFrequency(uint64_t frequency) const
  {
    return frequency >= m_lowestFrequency && frequency <= m_highestFrequency;
  }

protected:
  std::string m_name;
  std::string m_label;
  int64_t m_lowestFrequency;
  int64_t m_highestFrequency;
  int64_t m_landingFrequency;
  int32_t m_defaultFineStep;
  int32_t m_defaultCoarseStep;
  Mode::Type m_defaultMode;


};
