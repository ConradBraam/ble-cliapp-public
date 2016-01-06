#ifndef BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_
#define BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_

#include <memory>
#include <minar/minar.h>
#include "CLICommand/CommandResponse.h"

struct AsyncProcedure {

    /**
     * @brief start a new procedure, variadic args will be forwarded to
     * ProcedureType constructor.
     *
     * @param args Args used to build the procedure
     * @tparam ProcedureType The type of procedure to start
     */
    template<typename ProcedureType, typename... Ts>
    friend void startProcedure(Ts&&... args) {
        auto proc = new ProcedureType(std::forward<Ts>(args)...);
        proc->start();
    }

protected:
    /**
     * @brief Construct an AsyncProcedure
     *
     * @param res The response wich will be written during the procedure life.
     * @param timeout The maximum amount of time before the procedure termination
     */
    AsyncProcedure(const std::shared_ptr<CommandResponse>& res, uint32_t timeout);

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
    std::shared_ptr<CommandResponse> response;

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


    minar::callback_handle_t timeoutHandle;
    uint32_t timeout;
};

#endif //BLE_CLIAPP_UTIL_ASYNC_PROCEDURE_
