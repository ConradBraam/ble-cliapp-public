#ifndef BLE_CLIAPP_GAP_V1_COMMANDS_H_
#define BLE_CLIAPP_GAP_V1_COMMANDS_H_

#include "CLICommand/CommandSuite.h"

class GapV1CommandSuiteDescription {

public:
    static const char* name() {
        return "gap_v1";
    }

    static const char* info() {
        return "All commands applicable to the Gap instance of this device";
    }

    static const char* man() {
        // FIXME
        return "gap_v1 <command> <command arguments>.";
    }

    // see implementation
    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_GAP_V1_COMMANDS_H_