//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_SINKCONTROLLER_H
#define CUTESDR_VK6HL_SINKCONTROLLER_H
#include "Controller.h"


class SinkController : public Controller, public RadioSettingsSink, public PttSink
{
};


#endif //CUTESDR_VK6HL_SINKCONTROLLER_H