#include "AsyncProcedure.h"


AsyncProcedure::AsyncProcedure(const std::shared_ptr<CommandResponse>& res, uint32_t t) :
    response(res), timeoutHandle(nullptr), timeout(t) {
}

AsyncProcedure::~AsyncProcedure() {
    if(timeoutHandle) {
        minar::Scheduler::cancelCallback(timeoutHandle);
    }
}

void AsyncProcedure::start() {
    if(doStart() == false) {
        terminate();
    }

    // register the timeout callback
    timeoutHandle = minar::Scheduler::postCallback(
        this, &AsyncProcedure::whenTimeout
    ).delay(minar::milliseconds(timeout)).getHandle();
}

void AsyncProcedure::terminate() {
    delete this;
}


void AsyncProcedure::whenTimeout() {
    doWhenTimeout();
    terminate();
}

void AsyncProcedure::doWhenTimeout() {
    // detach whenConnected handle
    timeoutHandle = nullptr;
    response->faillure("timeout");
}
