#include "../EventDispatcher.h"

#include <qcoreapplication.h>

void EventDispatcher::postEvent(EventTarget* target, EventBase* event)
{
  QCoreApplication::postEvent(target, event);
}
