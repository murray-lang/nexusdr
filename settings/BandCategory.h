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

  const Band* nextBand(const std::string& name) const
  {
    auto it = std::ranges::find_if(m_bands, [&](const Band& b) {
      return b.getName() == name;
    });

    if (it != m_bands.end()) {
      auto index = std::distance(m_bands.begin(), it);
      if (index + 1 < m_bands.size()) {
        return &m_bands[index + 1];
      } else {
        return &m_bands[0];
      }
    }
    return nullptr;
  }

  const Band* prevBand(const std::string& name) const
  {
    auto it = std::ranges::find_if(m_bands, [&](const Band& b) {
      return b.getName() == name;
    });

    if (it != m_bands.end()) {
      auto index = std::distance(m_bands.begin(), it);
      if (index > 0) {
        return &m_bands[index - 1];
      } else {
        return &m_bands[m_bands.size()-1];
      }
    }
    return nullptr;
  }

protected:
  std::string m_name;
  std::string m_label;
  std::vector<Band> m_bands;
};
