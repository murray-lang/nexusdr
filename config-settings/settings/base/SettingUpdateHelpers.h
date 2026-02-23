//
// Created by murray on 23/2/26.
//

#pragma once
#include "SettingUpdate.h"


class SettingUpdateHelpers
{
public:
  static SettingUpdate makeSetBand(const std::string& bandName);
  static SettingUpdate makeSetMultiPipeline(SplitBandId whichBand, bool enable);
  static SettingUpdate makeClosePipeline(SplitBandId whichBand, PipelineId whichPipeline);
  static SettingUpdate makeSetTxPipeline(SplitBandId whichBand, PipelineId whichPipeline);
  static SettingUpdate makeSetFocusPipeline(SplitBandId whichBand, PipelineId whichPipeline);
  static SettingUpdate makeSetTxBand(SplitBandId whichBand);
  static SettingUpdate makeSetAgcSpeedOnFocusPipeline(SplitBandId whichBand, AgcSpeed speed);
  static SettingUpdate makeSetAgcSpeedOnFocusPipeline(AgcSpeed speed);

  // Applies to the *focused* pipeline within that band (assumes focus is already correct).
  static SettingUpdate makeSetModeOnFocusPipeline(SplitBandId whichBand, Mode::Type modeType);

protected:
  static uint32_t toWithBandFeature(SplitBandId whichBand);
};


