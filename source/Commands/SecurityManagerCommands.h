#ifndef BLE_CLIAPP_SECURITY_MANAGER_COMMANDS_H_
#define BLE_CLIAPP_SECURITY_MANAGER_COMMANDS_H_

#include "CLICommand/CommandSuite.h"

class SecurityManagerCommandSuiteDescription {

public:
    static const char* name() {
        return "securityManager";
    }

    static const char* info() {
        return "All commands applicable to the securityManager instance of this device";
    }

    static const char* man() {
        return "securityManager list: list all the commands available in this module\r\n"\
               "securityManager help <commandName>: display help available for a specific command\r\n"\
               "securityManager args <commandName>: display description of arguments expected for this command\r\n";
    }

    static ConstArray<const CommandTable*> commands();
};

#endif //BLE_CLIAPP_SECURITY_MANAGER_COMMANDS_H_
