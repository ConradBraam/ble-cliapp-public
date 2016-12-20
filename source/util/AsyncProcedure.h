#ifndef BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_
#define BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_

#include <core-util/SharedPointer.h>
#include "EventQueue/EventQueue.h"
#include "CLICommand/CommandResponse.h"

struct AsyncProcedure {

    template<typename ProcedureType, typename T0>
    friend void startProcedure(const T0& arg0);

    template<typename ProcedureType, typename T0, typename T1>
    friend void startProcedure(const T0& arg0, const T1& arg1);

    template<typename ProcedureType, typename T0, typename T1, typename T2>
    friend void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2);

    template<typename ProcedureType, typename T0, typename T1, typename T2, typename T3>
    friend void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3);

    template<typename ProcedureType, typename T0, typename T1, typename T2, typename T3, typename T4>
    friend void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4);

    template<typename ProcedureType, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    friend void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5);

protected:

    /**
     * @brief Construct an AsyncProcedure
     *
     * @param res The response wich will be written during the procedure life.
     * @param timeout The maximum amount of time before the procedure termination
     */
    AsyncProcedure(const CommandResponsePtr& res, uint32_t timeout);

    /**
     * @brief destructor for a procedure.
     */
    virtual ~AsyncProcedure();

    /**
     * @brief terminate the procedure any subsequent accedd to procedure member or
     * call to procedure member function is undefined.
     */
    void terminate();

    /**
     * @brief Implementation of start, implementer should return true if the
     * procedure was successfully launch and false otherwise
     * @return true if the procedure was successfully launch and false otherwise
     */
    virtual bool doStart() = 0;

    /**
     * @brief Called when a timeout occur
     */
    virtual void doWhenTimeout();

    /**
     * @brief response of the procedure
     */
    CommandResponsePtr response;

private:
    /**
     * @brief start the procedure, it will call doStart. If doStart return false,
     * it will terminate the procedure.
     */
    void start();

    /**
     * @brief [brief description]
     * @details [long description]
     */
    void whenTimeout();


    eq::EventQueue::event_handle_t timeoutHandle;
    uint32_t timeout;
};

/**
 * @brief start a new procedure, variadic args will be forwarded to
 * ProcedureType constructor.
 *
 * @param args Args used to build the procedure
 * @tparam ProcedureType The type of procedure to start
 */
template<typename ProcedureType, typename T0>
void startProcedure(T0& arg0) {
    ProcedureType* proc = new ProcedureType(arg0);
    proc->start();
}

template<typename ProcedureType, typename T0, typename T1>
void startProcedure(const T0& arg0, const T1& arg1) {
    ProcedureType* proc = new ProcedureType(arg0, arg1);
    proc->start();
}

template<typename ProcedureType, typename T0, typename T1, typename T2>
void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2) {
    ProcedureType* proc = new ProcedureType(arg0, arg1, arg2);
    proc->start();
}

template<typename ProcedureType, typename T0, typename T1, typename T2, typename T3>
void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3) {
    ProcedureType* proc = new ProcedureType(arg0, arg1, arg2, arg3);
    proc->start();
}

template<typename ProcedureType, typename T0, typename T1, typename T2, typename T3, typename T4>
void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4) {
    ProcedureType* proc = new ProcedureType(arg0, arg1, arg2, arg3, arg4);
    proc->start();
}

template<typename ProcedureType, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
void startProcedure(const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5) {
    ProcedureType* proc = new ProcedureType(arg0, arg1, arg2, arg3, arg4, arg5);
    proc->start();
}


#endif //BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_
