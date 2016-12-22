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
        return "BLE <command> <command arguments>.";
    }

    // see implementation
    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_BLE_COMMANDS_H_
