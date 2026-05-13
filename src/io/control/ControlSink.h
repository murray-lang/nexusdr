#pragma once


#include "ControlBase.h"
#include "PttSink.h"
#include "core/config-settings/settings/RadioSettingsSink.h"

class ControlSink : public ControlBase, public RadioSettingsSink, public PttSink
{
};
