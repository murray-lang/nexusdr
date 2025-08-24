//
// Created by murray on 20/8/25.
//

#ifndef CONTROLSINK_H
#define CONTROLSINK_H
#include "ControlBase.h"


class ControlSink : public ControlBase, public RadioSettingsSink, public PttSink
{
};


#endif //CONTROLSINK_H