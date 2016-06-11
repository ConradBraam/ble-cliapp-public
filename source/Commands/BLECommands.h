#ifndef BLE_CLIAPP_BLE_COMMANDS_H_
#define BLE_CLIAPP_BLE_COMMANDS_H_

#include "CLICommand/CommandSuite.h"
#include "ble/BLE.h"

class BLECommandSuiteDescription {

public:
    static const char* name() {
        return "ble";
    }

    static const char* info() {
        return "All commands applicable to BLE instance";
    }

    static const char* man() {
        return "BLE <command> <command arguments>\r\n"\
               "    * ble shutdown : shutdown the ble instance of this device\r\n"\
               "    * ble init : initialise the ble instance of this device\r\n"\
               "    * ble reset : reset the ble instance of this device\r\n"\
               "    * ble get_version : return the current version of the BLE stack\r\n";
    }

    // see implementation
    static ConstArray<CommandAccessor_t> commands();
};

#endif //BLE_CLIAPP_BLE_COMMANDS_H_
