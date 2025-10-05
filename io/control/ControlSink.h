//
// Created by murray on 20/8/25.
//

#ifndef CONTROLSINK_H
#define CONTROLSINK_H

#include "ControlBase.h"
#include "settings/PttSink.h"
#include "settings/RadioSettingsSink.h"

class ControlSink : public ControlBase, public RadioSettingsSink, public PttSink
{
};


#endif //CONTROLSINK_H