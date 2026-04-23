//
// Created by murray on 15/04/23.
//

#pragma once
#include "ResultCode.h"


class ControlBase {
public:
  virtual ~ControlBase() = default;

  virtual bool discover() = 0;
  virtual ResultCode open() = 0;
  virtual void close() = 0;
  virtual void exit() = 0;
};
