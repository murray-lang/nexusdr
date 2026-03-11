//
// Created by murray on 18/08/25.
//

#ifndef RECEIVERSETTINGSSINK_H
#define RECEIVERSETTINGSSINK_H
#include "ReceiverSettings.h"

class ReceiverSettingsSink
{
public:
  virtual ~ReceiverSettingsSink() = default;
  virtual void apply(const ReceiverSettings& rxSettings) = 0;
};
#endif //RECEIVERSETTINGSSINK_H
