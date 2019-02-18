#ifndef BLE_CLIAPP_GAP_V2_COMMANDS_H_
#define BLE_CLIAPP_GAP_V1_COMMANDS_H_

#include "CLICommand/CommandSuite.h"

class GapV2CommandSuiteDescription {

public:
    static const char* name() {
        return "gap_v2";
    }

    static const char* info() {
        return "All commands applicable to the Gap instance of this device";
    }

    static const char* man() {
        // FIXME
        return "gap_v2 <command> <command arguments>.";
    }

    // see implementation
    static ConstArray<const Command*> commands();

    static void init();
};

#endif //BLE_CLIAPP_GAP_V2_COMMANDS_H_
