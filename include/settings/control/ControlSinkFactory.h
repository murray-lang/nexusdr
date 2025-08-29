//
// Created by murray on 6/08/25.
//

#ifndef CONTROLSINKFACTORY_H
#define CONTROLSINKFACTORY_H
#include <string>

#include <config/ControlBaseConfig.h>
#include "ControlSink.h"


class ControlSinkFactory
{
public:
  static ControlSink* create(const ControlBaseConfig& config);
  static ControlSink* create(const std::string& type);
  // static std::string toLowerCase(const std::string& str);
};

#endif //CONTROLSINKFACTORY_H
