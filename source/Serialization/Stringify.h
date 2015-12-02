#ifndef BLE_CLIAPP_BLE_STRINGIFY_H_
#define BLE_CLIAPP_BLE_STRINGIFY_H_

#include "ble/blecommon.h"
#include "mbed-client-cli/ns_cmdline.h"

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