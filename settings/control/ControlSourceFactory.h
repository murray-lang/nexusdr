//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_CONTROLSOURCEFACTORY_H
#define CUTESDR_VK6HL_CONTROLSOURCEFACTORY_H
#include "ControlSource.h"
#include <config/ConfigBase.h>


class ControlSourceFactory
{
public:
  static ControlSource* create(const ConfigBase* pConfig);
  static ControlSource* create(const std::string& type);
};


#endif //CUTESDR_VK6HL_CONTROLSOURCEFACTORY_H