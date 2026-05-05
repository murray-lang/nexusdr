//
// Created by murray on 30/12/25.
//
#pragma once

#include "Band.h"
#include "CrossPlatformTypes.h"

#ifdef USE_ETL
#include <vector>

using BandsVector = etl::vector<Band, MAX_BANDS>;
#else
#include <vector>
using BandsVector = std::vector<Band>;
#endif

class BandCategory
{
public:
  BandCategory(const BandCategoryNameString& name, const BandCategoryLabelString& label, const BandsVector& bands) :
    m_name(name), m_label(label), m_bands(bands)
  {}

  BandCategory& add(const Band& band)
  {
    m_bands.push_back(band);
    return *this;
  }

  [[nodiscard]] const BandCategoryNameString& getName() const { return m_name; }
  [[nodiscard]] const BandCategoryLabelString& getLabel() const { return m_label; }
  [[nodiscard]] const BandsVector& getBands() const { return m_bands; }

  [[nodiscard]] const Band* findBand(const BandNameString& name) const
  {
    for (const auto& band : m_bands) {
      if (band.getName() == name) {
        return &band;
      }
    }
    return nullptr;
  }
  [[nodiscard]] const Band* findBand(uint64_t frequency) const
  {
    for (const auto& band : m_bands) {
      if (band.containsFrequency(frequency)) {
        return &band;
      }
    }
    return nullptr;
  }

  [[nodiscard]] const Band* nextBand(const BandNameString& name) const
  {
    auto it = find_if(m_bands.begin(), m_bands.end(), [&](const Band& b) {
      return b.getName() == name;
    });

    if (it != m_bands.end()) {
      auto index = distance(m_bands.begin(), it);
      if (index + 1 < m_bands.size()) {
        return &m_bands[index + 1];
      } else {
        return &m_bands[0];
      }
    }
    return nullptr;
  }

  const Band* prevBand(const BandNameString& name) const
  {
    auto it = find_if(m_bands.begin(), m_bands.end(), [&](const Band& b) {
      return b.getName() == name;
    });

    if (it != m_bands.end()) {
      auto index = distance(m_bands.begin(), it);
      if (index > 0) {
        return &m_bands[index - 1];
      } else {
        return &m_bands[m_bands.size()-1];
      }
    }
    return nullptr;
  }

protected:
  BandCategoryNameString m_name;
  BandCategoryLabelString m_label;
  BandsVector m_bands;
};
