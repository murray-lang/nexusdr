//
// Created by murray on 23/2/26.
//

#include "SettingUpdateHelpers.h"

#include "config-settings/settings/RadioSettings.h"

uint32_t SettingUpdateHelpers::toWithBandFeature(SplitBandId whichBand)
{
  switch (whichBand) {
  case SplitBandId::One: return BandSelector::WITH_1;
  case SplitBandId::Two: return BandSelector::WITH_2;
  default: break;
  }
  throw SettingsException("Invalid SplitBandId for band-scoped update");
}

SettingUpdate
SettingUpdateHelpers::makeSetBand(const std::string& bandName)
{
  SettingUpdatePath path({RadioSettings::BAND, BandSelector::REPLACE_FOCUS});
  return { path, bandName, SettingUpdate::Meaning::VALUE};
}

SettingUpdate SettingUpdateHelpers::makeSetMultiPipeline(SplitBandId whichBand, bool enable)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    toWithBandFeature(whichBand),
    BandSettings::MULTI_PIPELINE
  });
  return { path, enable, SettingUpdate::Meaning::VALUE };
}

SettingUpdate SettingUpdateHelpers::makeClosePipeline(SplitBandId whichBand, PipelineId whichPipeline)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    toWithBandFeature(whichBand),
    BandSettings::CLOSE_PIPELINE
  });
  return { path, whichPipeline, SettingUpdate::Meaning::VALUE};
}

SettingUpdate SettingUpdateHelpers::makeSetTxPipeline(SplitBandId whichBand, PipelineId whichPipeline)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    toWithBandFeature(whichBand),
    BandSettings::TX_PIPELINE
  });
  return { path, whichPipeline, SettingUpdate::Meaning::VALUE};
}

SettingUpdate SettingUpdateHelpers::makeSetFocusPipeline(SplitBandId whichBand, PipelineId whichPipeline)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    toWithBandFeature(whichBand),
    BandSettings::FOCUS_PIPELINE
  });
  return {path, whichPipeline, SettingUpdate::Meaning::VALUE};
}

SettingUpdate SettingUpdateHelpers::makeSetModeOnFocusPipeline(SplitBandId whichBand, Mode::Type modeType)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    toWithBandFeature(whichBand),
    BandSettings::WITH_FOCUS_PIPELINE,
    PipelineSettings::MODE
  });
  return {path, modeType, SettingUpdate::Meaning::VALUE};
}

SettingUpdate SettingUpdateHelpers::makeSetTxBand(SplitBandId whichBand)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    BandSelector::TX_BAND
  });
  return {path, whichBand, SettingUpdate::Meaning::VALUE};
}

SettingUpdate SettingUpdateHelpers::makeSetAgcSpeedOnFocusPipeline(SplitBandId whichBand, AgcSpeed speed)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    toWithBandFeature(whichBand),
    BandSettings::WITH_FOCUS_PIPELINE,
    RxPipelineSettings::AGC
  });
  return {path, speed, SettingUpdate::Meaning::VALUE};
}

SettingUpdate SettingUpdateHelpers::makeSetAgcSpeedOnFocusPipeline(AgcSpeed speed)
{
  SettingUpdatePath path({
    RadioSettings::BAND,
    BandSelector::WITH_FOCUS,
    BandSettings::WITH_FOCUS_PIPELINE,
    RxPipelineSettings::AGC
  });
  return {path, speed, SettingUpdate::Meaning::VALUE};
}