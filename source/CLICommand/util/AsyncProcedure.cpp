#include "AsyncProcedure.h"
#include "CommandEventQueue.h"

AsyncProcedure::AsyncProcedure(const CommandResponsePtr& res, uint32_t t) :
    response(res), timeoutHandle(NULL), timeout(t) {
}

AsyncProcedure::~AsyncProcedure() {
    if(timeoutHandle) {
        getCLICommandEventQueue()->cancel(timeoutHandle);
    }
}

void AsyncProcedure::start() {
    // register the timeout callback
    timeoutHandle = getCLICommandEventQueue()->post_in(
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
