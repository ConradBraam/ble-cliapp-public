//
// Created by Vincent Coubard on 2018-12-03.
//

#ifndef BLE_CLIAPP_CONNECTIONPARAMETERS_H
#define BLE_CLIAPP_CONNECTIONPARAMETERS_H

#include "ble/gap/ConnectionParameters.h"
#include "CLICommand/CommandSuite.h"

class ConnectionParametersCommandSuiteDescription {

public:
    static const char* name() {
        return "connectionParams";
    }

    static const char* info() {
        return "All commands applicable to the ConnectionParameters instance of this device";
    }

    static const char* man() {
        return "connectionParams <command> <command arguments>.";
    }

    static const ble::ConnectionParameters& get();

    // see implementation
    static ConstArray<const Command*> commands();
};

#endif //BLE_CLIAPP_CONNECTIONPARAMETERS_H
