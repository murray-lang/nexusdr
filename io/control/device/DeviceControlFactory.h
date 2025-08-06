//
// Created by murray on 6/08/25.
//

#ifndef DEVICECONTROLFACTORY_H
#define DEVICECONTROLFACTORY_H
#include <string>

#include "../../../radio/config/ControlConfig.h"
#include "DeviceControl.h"


class DeviceControlFactory
{
public:
  static DeviceControl* create(const ControlConfig& config);
  static DeviceControl* create(const std::string& type);
  static std::string toLowerCase(const std::string& str);
};

#endif //DEVICECONTROLFACTORY_H
