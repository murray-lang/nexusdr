#pragma once
#include "Bands.h"
#include <unordered_map>
#include "BandSettings.h"

#ifdef USE_ETL
#include <etl/unordered_map.h>

using BandSettingsMap = etl::unordered_map<BandNameString, BandSettings, MAX_BANDS>;
#else
#include <unordered_map>

using BandSettingsMap = std::unordered_map<BandNameString, BandSettings>;
#endif

class BandSelector
{
public:
  BandSelector();

  void initialiseCache();

  [[nodiscard]] const Bands& getAllBands() const { return m_bands; }
  BandSettings* getBandSettings(const BandNameString& bandName);
  [[nodiscard]] const BandSettings* getBandSettings(const BandNameString& bandName) const;
  void setCentreFrequencyDeltas(int32_t fine, int32_t coarse);
  void applyRfSettings(const RfSettings& settings, bool onlyChanged);
  void applyIfSettings(const IfSettings& settings);

  BandSettings* getOrCreateBandSettings(const BandNameString& bandName);

protected:
  Bands m_bands;
  BandSettingsMap m_bandSettingsCache;
};

