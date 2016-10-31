#ifndef BLE_CLIAPP_COMMANDS_COMMON_H_
#define BLE_CLIAPP_COMMANDS_COMMON_H_

#include "CLICommand/CommandSuite.h"
#include "ble/BLE.h"
#include <core-util/SharedPointer.h>

/**
 * return the ble instance of this device
 */
inline BLE& ble() {
    return BLE::Instance();
}

/**
 * return the Gap instance of this device
 */
inline Gap& gap() {
    return ble().gap();
}


/**
 * @brief Report an error or a success for a command
 * 
 * @param response The response used to report the status.
 * @param err Generic ble error.
 */
inline void reportErrorOrSuccess(const mbed::util::SharedPointer<CommandResponse>& response, ble_error_t err) {
    if(err) {
        response->faillure(err);
    } else {
        response->success();
    }
}


/**
 * @brief Report an error or a success for a command. In case of success, the 
 * parameter res is streamed to the response.
 * 
 * @param response The response used to report the status.
 * @param err Generic ble error.
 * @param res The result to stream in case of success.
 */
template<typename T>
void reportErrorOrSuccess(const mbed::util::SharedPointer<CommandResponse>& response, ble_error_t err, const T& res) {
    if(err) {
        response->faillure(err);
    } else {
        response->success(res);
    }
}


#endif //BLE_CLIAPP_COMMANDS_COMMON_H_
