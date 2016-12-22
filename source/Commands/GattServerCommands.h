#ifndef BLE_CLIAPP_GATT_SERVER_COMMANDS_H_
#define BLE_CLIAPP_GATT_SERVER_COMMANDS_H_

#include "CLICommand/CommandSuite.h"

class GattServerCommandSuiteDescription {

public:
    static const char* name() {
        return "gattServer";
    }

    static const char* info() {
        return "All commands applicable to the gattServer instance of this device";
    }

    static const char* man() {
        return "gattServer <command> <command arguments>.";
    }

    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_GATT_SERVER_COMMANDS_H_
