//
// Created by murray on 30/12/25.
//

#pragma once

#include <vector>
#include "BandCategory.h"

#ifdef USE_ETL
#include <etl/vector.h>
#include <etl/string.h>

using BandCategoriesVector = etl::vector<BandCategory, MAX_BAND_CATEGORIES>;

#else
#include <vector>
#include <string>

using BandCategoriesVector = std::vector<BandCategory>;
#endif

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

  const BandCategoriesVector& getCategories() const { return m_categories; }

  const BandCategory* findCategory(const BandCategoryNameString& categoryName) const
  {
    for (const auto& cat : m_categories) {
      if (cat.getName() == categoryName) {
        return &cat;
      }
    }
    return nullptr;
  }

  int32_t findCategoryIndex(const BandCategoryNameString& categoryName) const
  {
    for (int i = 0; i < m_categories.size(); i++) {
      if (m_categories[i].getName() == categoryName) {
        return i;
      }
    }
    return -1;
  }

  const BandCategory* findCategoryOfBand(const BandNameString& bandName) const
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

  const Band* findBand(const BandNameString& bandName) const
  {
    for (const auto& cat : m_categories) {
      const Band* band = cat.findBand(bandName);
      if (band != nullptr) {
        return band;
      }
    }
    return nullptr;
  }

  const Band* nextBandInOwnCategory(const BandNameString& bandName) const
  {
    const auto& category = findCategoryOfBand(bandName);
    if (category == nullptr) {
      return nullptr;
    }
    return category->nextBand(bandName);
  }

  const Band* prevBandInOwnCategory(const BandNameString& bandName) const
  {
    const auto& category = findCategoryOfBand(bandName);
    if (category == nullptr) {
      return nullptr;
    }
    return category->prevBand(bandName);
  }

protected:
  BandCategoriesVector m_categories;
};
