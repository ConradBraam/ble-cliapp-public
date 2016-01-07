#include "AsyncProcedure.h"

AsyncProcedure::AsyncProcedure(const mbed::util::SharedPointer<CommandResponse>& res, std::uint32_t t) :
    response(res), timeoutHandle(nullptr), timeout(t) {
}

AsyncProcedure::~AsyncProcedure() {
    if(timeoutHandle) {
        minar::Scheduler::cancelCallback(timeoutHandle);
    }
}

void AsyncProcedure::start() {
    // register the timeout callback
    timeoutHandle = minar::Scheduler::postCallback(
        this, &AsyncProcedure::whenTimeout
    ).delay(minar::milliseconds(timeout)).getHandle();

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
    timeoutHandle = nullptr;
    doWhenTimeout();
    terminate();
}

void AsyncProcedure::doWhenTimeout() {
    response->faillure("timeout");
}
