//
// Created by Vincent Coubard on 2018-11-30.
//

#ifndef BLE_CLIAPP_ADVERTISINGPARAMETERS_H
#define BLE_CLIAPP_ADVERTISINGPARAMETERS_H

#include "ble/gap/AdvertisingParameters.h"
#include "CLICommand/CommandSuite.h"

class AdvertisingParametersCommandSuiteDescription {

public:
    static const char* name() {
        return "advParams";
    }

    static const char* info() {
        return "All commands applicable to the AdvertisingParameters instance of this device";
    }

    static const char* man() {
        return "advParams <command> <command arguments>.";
    }

    static const ble::AdvertisingParameters& get();

    // see implementation
    static ConstArray<const Command*> commands();
};


#endif //BLE_CLIAPP_ADVERTISINGPARAMETERS_H
