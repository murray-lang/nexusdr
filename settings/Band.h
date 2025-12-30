//
// Created by murray on 30/12/25.
//

#pragma once
#include <cstdint>
#include <string>

#include "Mode.h"

class Band
{
public:
  Band() :
    m_name(""),
    m_label(""),
    m_lowestFrequency(0),
    m_highestFrequency(0),
    m_landingFrequency(0),
    m_defaultMode(Mode::Type::NONE)
  {}

  Band(
    const std::string& name,
    const std::string& label,
    uint64_t lowestFrequency,
    uint64_t highestFrequency,
    uint64_t landingFrequency,
    Mode::Type defaultMode
  ) :
    m_name(name),
    m_label(label),
    m_lowestFrequency(lowestFrequency),
    m_highestFrequency(highestFrequency),
    m_landingFrequency(landingFrequency),
    m_defaultMode(defaultMode)
  {}
  Band(const Band& rhs) = default;
  ~Band() = default;

  Band& operator=(const Band& rhs) = default;

  Band& invalidate()
  {
    m_name = "";
    m_label = "";
    m_lowestFrequency = 0;
    m_highestFrequency = 0;
    m_landingFrequency = 0;
    m_defaultMode = Mode::Type::NONE;
    return *this;
  }

  bool isValid() const { return m_name.size() > 0; }

  const std::string& getName() const { return m_name; }
  const std::string& getLabel() const { return m_label; }
  uint64_t getLowestFrequency() const { return m_lowestFrequency; }
  uint64_t getHighestFrequency() const { return m_highestFrequency; }
  uint64_t getLandingFrequency() const { return m_landingFrequency; }
  Mode::Type getDefaultMode() const { return m_defaultMode; }

  bool containsFrequency(uint64_t frequency) const
  {
    return frequency >= m_lowestFrequency && frequency <= m_highestFrequency;
  }

protected:
  std::string m_name;
  std::string m_label;
  uint64_t m_lowestFrequency;
  uint64_t m_highestFrequency;
  uint64_t m_landingFrequency;
  Mode::Type m_defaultMode;


};
