//
// Created by murray on 15/04/23.
//

#pragma once


#include <string>
#include <nlohmann/json.hpp>

#include "../../config-settings/config/ConfigBase.h"

class ControlBase {
public:
  ControlBase() = default;
  virtual ~ControlBase() = default;

  virtual void configure(const ConfigBase* pConfig) = 0;
  virtual bool discover() = 0;
  virtual void open() = 0;
  virtual void close() = 0;
  virtual void exit() = 0;

  const std::string& getId() { return id; }
  void setId(const std::string& newId ) { id = newId; }

protected:
    std::string id;
};
