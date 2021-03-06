#ifndef BLE_CLIAPP_GAP_COMMANDS_H_
#define BLE_CLIAPP_GAP_COMMANDS_H_

#include "CLICommand/CommandSuite.h"

class GapCommandSuiteDescription {

public:
    static const char* name() {
        return "gap";
    }

    static const char* info() {
        return "All commands applicable to the Gap instance of this device";
    }

    static const char* man() {
        return "gap <command> <command arguments>.";
    }

    // see implementation
    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_GAP_COMMANDS_H_
