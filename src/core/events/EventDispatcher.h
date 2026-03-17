//
// Created by murray on 26/2/26.
//

#pragma once

#include "EventBase.h"
#include "EventTarget.h"

class EventDispatcher
{
public:
    static void postEvent(EventTarget* target, EventBase* event);
};
