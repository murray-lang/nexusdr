//
// Created by murray on 30/12/25.
//
#pragma once

#include "Band.h"
#include <vector>
#include <string>

class BandCategory
{
public:
  BandCategory(const std::string& name, const std::string& label, const std::vector<Band>& bands) :
    m_name(name), m_label(label), m_bands(bands)
  {}

  BandCategory& add(const Band& band)
  {
    m_bands.push_back(band);
    return *this;
  }

  const std::string& getName() const { return m_name; }
  const std::string& getLabel() const { return m_label; }
  const std::vector<Band>& getBands() const { return m_bands; }

  const Band* findBand(const std::string& name) const
  {
    for (const auto& band : m_bands) {
      if (band.getName() == name) {
        return &band;
      }
    }
    return nullptr;
  }
  const Band* findBand(uint64_t frequency) const
  {
    for (const auto& band : m_bands) {
      if (band.containsFrequency(frequency)) {
        return &band;
      }
    }
    return nullptr;
  }

protected:
  std::string m_name;
  std::string m_label;
  std::vector<Band> m_bands;
};
