#ifndef BLE_CLIAPP_BLE_STRINGIFY_H_
#define BLE_CLIAPP_BLE_STRINGIFY_H_

#include "ble/blecommon.h"
#include "mbed-client-cli/ns_cmdline.h"

static inline const char* to_string(ble_error_t err) {
    switch(err) {
        case BLE_ERROR_NONE: 
            return "BLE_ERROR_NONE";
        case BLE_ERROR_BUFFER_OVERFLOW: 
            return "BLE_ERROR_BUFFER_OVERFLOW";
        case BLE_ERROR_NOT_IMPLEMENTED: 
            return "BLE_ERROR_NOT_IMPLEMENTED";
        case BLE_ERROR_PARAM_OUT_OF_RANGE: 
            return "BLE_ERROR_PARAM_OUT_OF_RANGE";
        case BLE_ERROR_INVALID_PARAM: 
            return "BLE_ERROR_INVALID_PARAM";
        case BLE_STACK_BUSY: 
            return "BLE_STACK_BUSY";
        case BLE_ERROR_INVALID_STATE: 
            return "BLE_ERROR_INVALID_STATE";
        case BLE_ERROR_NO_MEM: 
            return "BLE_ERROR_NO_MEM";
        case BLE_ERROR_OPERATION_NOT_PERMITTED: 
            return "BLE_ERROR_OPERATION_NOT_PERMITTED";
        case BLE_ERROR_UNSPECIFIED: 
            return "BLE_ERROR_UNSPECIFIED"; 
        default:
            return "Unknown ble_error_t";
    }
};

static inline const char* cmdline_retcode_to_string(int code) {
    switch(code) {
        case CMDLINE_RETCODE_COMMAND_BUSY:
            return "CMDLINE_RETCODE_COMMAND_BUSY";
        case CMDLINE_RETCODE_EXCUTING_CONTINUE:
            return "CMDLINE_RETCODE_EXCUTING_CONTINUE";
        case CMDLINE_RETCODE_SUCCESS:
            return "CMDLINE_RETCODE_SUCCESS";
        case CMDLINE_RETCODE_FAIL:
            return "CMDLINE_RETCODE_FAIL";
        case CMDLINE_RETCODE_INVALID_PARAMETERS:
            return "CMDLINE_RETCODE_INVALID_PARAMETERS";
        case CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED:
            return "CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED";
        case CMDLINE_RETCODE_COMMAND_CB_MISSING:
            return "CMDLINE_RETCODE_COMMAND_CB_MISSING";
        case CMDLINE_RETCODE_COMMAND_NOT_FOUND:
            return "CMDLINE_RETCODE_COMMAND_NOT_FOUND";
        default:
            return "CMDLINE_RETCODE_UNKNOWN";
    }    
}

#endif //BLE_CLIAPP_BLE_STRINGIFY_H_