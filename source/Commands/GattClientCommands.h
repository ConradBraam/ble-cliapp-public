#ifndef BLE_CLIAPP_GATT_CLIENT_COMMANDS_H_
#define BLE_CLIAPP_GATT_CLIENT_COMMANDS_H_

#include "CLICommand/CommandSuite.h"

class GattClientCommandSuiteDescription {

public:
    static const char* name() {
        return "gattClient";
    }

    static const char* info() {
        return "All commands applicable to the gattClient instance of this device";
    }

    static const char* man() {
        return "gattClient <command> <command arguments>.";
    }

    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_GATT_CLIENT_COMMANDS_H_
