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

protected:
  std::vector<BandCategory> m_categories;
};
