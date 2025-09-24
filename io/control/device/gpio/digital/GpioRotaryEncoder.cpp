//
// Created by murray on 20/8/25.
//

#include "GpioRotaryEncoder.h"
#include <qdebug.h>

#include "io/control/device/gpio/GpioException.h"

void
GpioRotaryEncoder::configure(const DigitalInputConfig* pConfig)
{
  DigitalInput::configure(pConfig);
}

void
GpioRotaryEncoder::notifyMovement(const int movement)
{
  SettingDelta delta(m_settingPath, movement);
  notifySingleSetting(delta);
}

bool
GpioRotaryEncoder::handleLineChange(GpioLines::LineStateMap& changedLines)
{
  auto aIter = changedLines.find(m_lines[0]);
  auto bIter = changedLines.find(m_lines[1]);
  if (aIter != changedLines.end() && bIter != changedLines.end()) {
    GpioLines::LineState& a = aIter->second;
    GpioLines::LineState& b = bIter->second;
    int dir = calculateMovement(a, b);
    qDebug() << "A:" << a.value << "B:" << b.value << "Direction: " << dir;
    notifyMovement(dir);
    return true;
  }
  return false;
}

int
GpioRotaryEncoder::calculateMovement(GpioLines::LineState& a, GpioLines::LineState& b)
{
  if (a.value ==  1 && b.value == 1) {
    return a.lastRisingTime < b.lastRisingTime ? +1 : -1;
  }
  if (a.value == 0 && b.value == 0) {
    return a.lastFallingTime < b.lastFallingTime ? +1 : -1;
  }
  if (a.value == 1 && b.value == 0) {
    return a.lastRisingTime < b.lastFallingTime ? -1 : +1;
  }
  if (a.value == 0 && b.value == 1) {
    return a.lastFallingTime < b.lastRisingTime ? -1 : +1;
  }
  return 0;

}
