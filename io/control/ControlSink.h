//
// Created by murray on 20/8/25.
//

#pragma once


#include "ControlBase.h"
#include "PttSink.h"
#include "config-settings/settings/RadioSettingsSink.h"

class ControlSink : public ControlBase, public RadioSettingsSink, public PttSink
{
};
