#pragma once

#ifdef IS_QT
#include <QEvent>

using EventType = QEvent::Type;
#endif

#ifdef IS_STM32
#endif