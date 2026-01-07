//
// Created by murray on 20/8/25.
//

#pragma once


#include "ControlBase.h"
#include "PttSink.h"
#include "settings/RadioAndBandSettingsSink.h"

class ControlSink : public ControlBase, public RadioAndBandSettingsSink, public PttSink
{
};
