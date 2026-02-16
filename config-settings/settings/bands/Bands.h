//
// Created by murray on 30/12/25.
//

#pragma once

#include <vector>
#include "BandCategory.h"

class Bands
{
public:
  Bands();
  // Bands(const std::vector<BandCategory>& categories): m_categories(categories)
  // {
  // }

  Bands& add(const BandCategory& category)
  {
    m_categories.push_back(category);
    return *this;
  }

  const std::vector<BandCategory>& getCategories() const { return m_categories; }

  const BandCategory* findCategory(const std::string& categoryName) const
  {
    for (const auto& cat : m_categories) {
      if (cat.getName() == categoryName) {
        return &cat;
      }
    }
    return nullptr;
  }

  int32_t findCategoryIndex(const std::string& categoryName) const
  {
    for (int i = 0; i < m_categories.size(); i++) {
      if (m_categories[i].getName() == categoryName) {
        return i;
      }
    }
    return -1;
  }

  const BandCategory* findCategoryOfBand(const std::string& bandName) const
  {
    for (const auto& cat : m_categories) {
      if (cat.findBand(bandName)) {
        return &cat;
      }
    }
    return nullptr;
  }

  const Band* findBand(uint64_t frequency) const
  {
    for (const auto& cat : m_categories) {
      const Band* band = cat.findBand(frequency);
      if (band != nullptr) {
        return band;
      }
    }
    return nullptr;
  }

  const Band* findBand(const std::string& bandName) const
  {
    for (const auto& cat : m_categories) {
      const Band* band = cat.findBand(bandName);
      if (band != nullptr) {
        return band;
      }
    }
    return nullptr;
  }

  const Band* nextBandInOwnCategory(const std::string& bandName) const
  {
    const auto& category = findCategoryOfBand(bandName);
    if (category == nullptr) {
      return nullptr;
    }
    return category->nextBand(bandName);
  }

  const Band* prevBandInOwnCategory(const std::string& bandName) const
  {
    const auto& category = findCategoryOfBand(bandName);
    if (category == nullptr) {
      return nullptr;
    }
    return category->prevBand(bandName);
  }

protected:
  std::vector<BandCategory> m_categories;
};
