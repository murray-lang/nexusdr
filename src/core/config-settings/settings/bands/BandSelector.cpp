//
// Created by murray on 17/3/26.
//

#include "BandSelector.h"

BandSelector::BandSelector()
{
  initialiseCache();
}

void
BandSelector::initialiseCache()
{
  // These will eventually be overridden by settings from JSON
  const std::vector<BandCategory>& categories = m_bands.getCategories();
  for (const auto& category : categories) {
    for (const auto& band : category.getBands()) {
      m_bandSettingsCache.emplace(band.getName(), BandSettings(band));
    }
  }
}

BandSettings*
BandSelector::getBandSettings(const std::string& bandName)
{
  if (m_bandSettingsCache.contains(bandName)) {
    return &m_bandSettingsCache.at(bandName);
  }
  return nullptr;
}

const BandSettings*
BandSelector::getBandSettings(const std::string& bandName) const
{
  if (m_bandSettingsCache.contains(bandName)) {
    return &m_bandSettingsCache.at(bandName);
  }
  return nullptr;
}

void
BandSelector::setCentreFrequencyDeltas(int32_t fine, int32_t coarse)
{
  for (auto& item : m_bandSettingsCache) {
    item.second.setCentreFrequencyDeltas(fine, coarse);
  }
  // m_changed |= WITH_FOCUS;
}

void
BandSelector::applyRfSettings(const RfSettings& settings, bool onlyChanged)
{
  for (auto& item : m_bandSettingsCache) {
    item.second.applyRfSettings(settings, onlyChanged);
  }
  // m_changed |= WITH_FOCUS;
}

void
BandSelector::applyIfSettings(const IfSettings& settings)
{
  for (auto& item : m_bandSettingsCache) {
    item.second.applyIfSettings(settings);
  }
  // m_changed |= WITH_FOCUS;
}

BandSettings*
BandSelector::getOrCreateBandSettings(const std::string& bandName)
{
  BandSettings* bandSettings = getBandSettings(bandName);
  if (bandSettings != nullptr) {
    return bandSettings;
  }
  if (const Band* bandInfo = m_bands.findBand(bandName)) {
    m_bandSettingsCache.emplace(bandName, BandSettings(*bandInfo));
    return &m_bandSettingsCache.at(bandName);
  } else {
    return nullptr;
  }
}