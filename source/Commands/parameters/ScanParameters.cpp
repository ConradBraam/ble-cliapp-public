//
// Created by Vincent Coubard on 2018-11-30.
//

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/BLECommonSerializer.h"
#include "CLICommand/CommandSuite.h"
#include "CLICommand/CommandHelper.h"

#include "ScanParameters.h"

namespace {

// global parameters being modified
ble::ScanParameters parameters;

DECLARE_CMD(Reset) {
    CMD_NAME("reset")
    CMD_HANDLER(CommandResponsePtr& response) {
        parameters = ble::ScanParameters();
        response->success();
    }
};

DECLARE_CMD(SetOwnAddressType) {
    CMD_NAME("setOwnAddressType")
    CMD_HANDLER(ble::own_address_type_t::type type, CommandResponsePtr& response) {
        parameters.setOwnAddressType(type);
        response->success();
    }
};

DECLARE_CMD(SetFilter) {
    CMD_NAME("setFilter")
    CMD_HANDLER(ble::scanning_filter_policy_t::type filter, CommandResponsePtr& response) {
        parameters.setFilter(filter);
        response->success();
    }
};

DECLARE_CMD(SetPhys) {
    CMD_NAME("setPhys")
    CMD_HANDLER(bool enable1M, bool enablePhy, CommandResponsePtr& response) {
        parameters.setPhys(enable1M, enablePhy);
        response->success();
    }
};

DECLARE_CMD(Set1mPhyConfiguration) {
    CMD_NAME("set1mPhyConfiguration")
    CMD_HANDLER(ble::scan_interval_t interval, ble::scan_window_t window, bool activeScanning, CommandResponsePtr& response) {
        parameters.set1mPhyConfiguration(interval, window, activeScanning);
        response->success();
    }
};

DECLARE_CMD(SetCodedPhyConfiguration) {
    CMD_NAME("setCodedPhyConfiguration")
    CMD_HANDLER(ble::scan_interval_t interval, ble::scan_window_t window, bool activeScanning, CommandResponsePtr& response) {
        parameters.setCodedPhyConfiguration(interval, window, activeScanning);
        response->success();
    }
};

}

DECLARE_SUITE_COMMANDS(ScanParametersCommandSuiteDescription,
    CMD_INSTANCE(Reset),
    CMD_INSTANCE(SetOwnAddressType),
    CMD_INSTANCE(SetFilter),
    CMD_INSTANCE(SetPhys),
    CMD_INSTANCE(Set1mPhyConfiguration),
    CMD_INSTANCE(SetCodedPhyConfiguration)
);

const ble::ScanParameters& ScanParametersCommandSuiteDescription::get() {
    return parameters;
}
