#pragma once

#include "CrossPlatformTypes.h"

#ifdef USE_ETL
#include <etl/map.h>
#include <etl/string.h>

using SettingPathString = etl::string<MAX_SETTING_PATH_LENGTH>;
using FeatureString = etl::string<MAX_SETTING_NAME_LENGTH>;
using FeatureStringVector = etl::vector<FeatureString, MAX_SETTING_DEPTH>;
using FeatureStringMap = etl::map<FeatureString, uint32_t, MAX_SETTINGS_TYPES>;
using FeatureNumVector = etl::vector<uint32_t, MAX_SETTING_DEPTH>;

using BandNameString = etl::string<MAX_BAND_NAME_LENGTH>;
using BandLabelString = etl::string<MAX_BAND_LABEL_LENGTH>;

using BandCategoryNameString = etl::string<MAX_BAND_CATEGORY_NAME_LENGTH>;
using BandCategoryLabelString = etl::string<MAX_BAND_CATEGORY_LABEL_LENGTH>;

using ModeNameString = etl::string<MAX_MODE_NAME_LENGTH>;
using ModeLabelString = etl::string<MAX_MODE_LABEL_LENGTH>;

#else
#include <map>
#include <string>
using SettingPathString = std::string;
using FeatureString = std::string;
using FeatureStringVector = std::vector<FeatureString>;
using FeatureStringMap = std::map<FeatureString, uint32_t>;
using FeatureNumVector = std::vector<uint32_t>;

// Strong types to distinguish in variants
struct BandNameString : std::string { using std::string::string; };
struct BandLabelString : std::string { using std::string::string; };
struct BandCategoryNameString : std::string { using std::string::string; };
struct BandCategoryLabelString : std::string { using std::string::string; };

struct ModeNameString : std::string { using std::string::string; };
struct ModeLabelString : std::string { using std::string::string; };

// Hash specializations for unordered_map support
namespace std {
  template<> struct hash<BandNameString> {
    size_t operator()(const BandNameString& s) const noexcept {
      return hash<std::string>{}(s);
    }
  };
  template<> struct hash<BandLabelString> {
    size_t operator()(const BandLabelString& s) const noexcept {
      return hash<std::string>{}(s);
    }
  };
  template<> struct hash<BandCategoryNameString> {
    size_t operator()(const BandCategoryNameString& s) const noexcept {
      return hash<std::string>{}(s);
    }
  };
  template<> struct hash<BandCategoryLabelString> {
    size_t operator()(const BandCategoryLabelString& s) const noexcept {
      return hash<std::string>{}(s);
    }
  };
  template<> struct hash<ModeNameString> {
    size_t operator()(const ModeNameString& s) const noexcept {
      return hash<std::string>{}(s);
    }
  };
  template<> struct hash<ModeLabelString> {
    size_t operator()(const ModeLabelString& s) const noexcept {
      return hash<std::string>{}(s);
    }
  };
}
#endif