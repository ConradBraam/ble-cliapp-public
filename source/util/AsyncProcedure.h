#ifndef BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_
#define BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_


struct AsyncProcedure {
    AsyncProcedure(const std::shared_ptr<CommandResponse>& res, uint32_t t) :
        response(res), timeoutHandle(nullptr), timeout(t) {
    }

    virtual ~AsyncProcedure() {
        if(timeoutHandle) {
            minar::Scheduler::cancelCallback(timeoutHandle);
        }
    }

    void start() {
        if(doStart() == false) {
            terminate();
        }

        // register the timeout callback
        timeoutHandle = minar::Scheduler::postCallback(
            this, &AsyncProcedure::whenTimeout
        ).delay(minar::milliseconds(timeout)).getHandle();
    }

    void terminate() {
        delete this;
    }

    virtual bool doStart() = 0;

    void whenTimeout() {
        doWhenTimeout();
        terminate();
    }

    virtual void doWhenTimeout() {
        // detach whenConnected handle
        timeoutHandle = nullptr;
        response->faillure("timeout");
    }

    std::shared_ptr<CommandResponse> response;
    minar::callback_handle_t timeoutHandle;
    uint32_t timeout;
};


template<typename ProcedureType, typename... Ts>
void startProcedure(Ts&&... args) {
    auto proc = new ProcedureType(std::forward<Ts>(args)...);
    proc->start();
}


#endif //BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_
