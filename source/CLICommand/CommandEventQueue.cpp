#include "CommandEventQueue.h"

static eq::EventQueue* _event_queue = NULL;

eq::EventQueue* getCLICommandEventQueue() { 
    return _event_queue;
}

void initCLICommandEventQueue(eq::EventQueue* event_queue) { 
    _event_queue = event_queue;
}