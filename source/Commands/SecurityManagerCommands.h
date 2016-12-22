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
        return "securityManager <command> <command arguments>.";
    }

    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_SECURITY_MANAGER_COMMANDS_H_
