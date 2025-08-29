//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_SOURCECONTROLLERFACTORY_H
#define CUTESDR_VK6HL_SOURCECONTROLLERFACTORY_H
#include "ControlSource.h"
#include <config/ControlBaseConfig.h>


class ControlSourceFactory
{
public:
  static ControlSource* create(const ControlBaseConfig& config);
  static ControlSource* create(const std::string& type);
};


#endif //CUTESDR_VK6HL_SOURCECONTROLLERFACTORY_H