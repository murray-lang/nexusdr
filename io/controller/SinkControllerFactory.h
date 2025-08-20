//
// Created by murray on 6/08/25.
//

#ifndef SINKCONTROLLERFACTORY_H
#define SINKCONTROLLERFACTORY_H
#include <string>

#include "config/ControllerConfig.h"
#include "SinkController.h"


class SinkControllerFactory
{
public:
  static SinkController* create(const ControllerConfig& config);
  static SinkController* create(const std::string& type);
  static std::string toLowerCase(const std::string& str);
};

#endif //SINKCONTROLLERFACTORY_H
