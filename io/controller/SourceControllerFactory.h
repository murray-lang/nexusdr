//
// Created by murray on 20/8/25.
//

#ifndef CUTESDR_VK6HL_SOURCECONTROLLERFACTORY_H
#define CUTESDR_VK6HL_SOURCECONTROLLERFACTORY_H
#include "SourceController.h"
#include "config/ControllerConfig.h"


class SourceControllerFactory
{
public:
  static SourceController* create(const ControllerConfig& config);
  static SourceController* create(const std::string& type);
  static std::string toLowerCase(const std::string& str);
};


#endif //CUTESDR_VK6HL_SOURCECONTROLLERFACTORY_H