//
// Created by murray on 26/2/26.
//

#pragma once

#ifdef IS_QT
#include <QEvent>

using EventBase = QEvent;
#endif

#ifdef IS_STM32
#endif