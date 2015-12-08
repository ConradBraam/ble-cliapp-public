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
        return "gattClient list: list all the commands available in this module\r\n"\
               "gattClient help <commandName>: display help available for a specific command\r\n"\
               "gattClient args <commandName>: display description of arguments expected for this command\r\n";
    } 

    static ConstArray<Command> commands();
};

#endif //BLE_CLIAPP_GATT_CLIENT_COMMANDS_H_