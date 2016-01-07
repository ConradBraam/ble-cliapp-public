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
        return "gattServer list: list all the commands available in this module\r\n"\
               "gattServer help <commandName>: display help available for a specific command\r\n"\
               "gattServer args <commandName>: display description of arguments expected for this command\r\n";
    }

    static ConstArray<Command> commands();
};

#endif //BLE_CLIAPP_GATT_SERVER_COMMANDS_H_
