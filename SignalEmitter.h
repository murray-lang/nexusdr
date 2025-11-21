//
// Created by murray on 15/03/25.
//

#pragma once

#include <QObject>
#include "dsp/utils/FftData.h"

class SignalEmitter : public QObject
{
  Q_OBJECT
public:
  typedef enum tagSignalStage
  {
    eSIGNAL_INPUT,
    eSIGNAL_MIXER,
    eSIGNAL_DECIMATOR,
    eSIGNAL_IF_FILTER,
    eSIGNAL_DEMODULATED
  } SignalStage;
signals:
  void signalComplexSignal(SignalStage stage, SharedComplexSeriesData signal, uint32_t length);
  void signalRealSignal(SignalStage stage, SharedRealSeriesData signal, uint32_t length);

public:
  void emitComplexSignal(SignalStage stage, const vsdrcomplex& signal, uint32_t length)
  {
    SharedComplexSeriesData sharedSignal = SharedComplexSeriesData(new vsdrcomplex(signal));
    emit signalComplexSignal(stage, sharedSignal, length);
  }

  void emitRealSignal(SignalStage stage, const vsdrreal& signal, uint32_t length)
  {
    SharedRealSeriesData sharedSignal = SharedRealSeriesData(new vsdrreal(signal));
    emit signalRealSignal(stage, sharedSignal, length);
  }
};
