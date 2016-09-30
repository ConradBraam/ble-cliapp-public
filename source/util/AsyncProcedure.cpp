#include "AsyncProcedure.h"

// TODO: ugly, should be injected
extern eq::EventQueue& taskQueue;

AsyncProcedure::AsyncProcedure(const mbed::util::SharedPointer<CommandResponse>& res, uint32_t t) :
    response(res), timeoutHandle(NULL), timeout(t) {
}

AsyncProcedure::~AsyncProcedure() {
    if(timeoutHandle) {
        taskQueue.cancel(timeoutHandle);
    }
}

void AsyncProcedure::start() {
    // register the timeout callback
    timeoutHandle = taskQueue.post_in(
        &AsyncProcedure::whenTimeout,
        this,
        timeout
    );

    if(doStart() == false) {
        terminate();
        return;
    }

}

void AsyncProcedure::terminate() {
    delete this;
}

void AsyncProcedure::whenTimeout() {
    // detach whenConnected handle
    timeoutHandle = NULL;
    doWhenTimeout();
    terminate();
}

void AsyncProcedure::doWhenTimeout() {
    response->faillure("timeout");
}
