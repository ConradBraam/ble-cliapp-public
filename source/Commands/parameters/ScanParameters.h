//
// Created by Vincent Coubard on 2018-11-30.
//

#ifndef BLE_CLIAPP_SCANPARAMETERS_H
#define BLE_CLIAPP_SCANPARAMETERS_H

#include "ble/gap/ScanParameters.h"
#include "CLICommand/CommandSuite.h"

class ScanParametersCommandSuiteDescription {

public:
    static const char* name() {
        return "scanParams";
    }

    static const char* info() {
        return "All commands applicable to the ScanParameters instance of this device";
    }

    static const char* man() {
        return "scanParams <command> <command arguments>.";
    }

    static const ble::ScanParameters& get();

    // see implementation
    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_SCANPARAMETERS_H
