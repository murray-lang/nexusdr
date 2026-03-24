//
// Created by murray on 17/3/26.
//

#pragma once
#include "Bands.h"
#include <unordered_map>
#include "BandSettings.h"

class BandSelector
{
public:
  BandSelector();

  void initialiseCache();

  const Bands& getAllBands() const { return m_bands; }
  BandSettings* getBandSettings(const std::string& bandName);
  const BandSettings* getBandSettings(const std::string& bandName) const;
  void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);
  void applyRfSettings(const RfSettings& settings, bool onlyChanged);
  void applyIfSettings(const IfSettings& settings);

  BandSettings* getOrCreateBandSettings(const std::string& bandName);

protected:
  Bands m_bands;
  std::unordered_map<std::string, BandSettings> m_bandSettingsCache;
};

