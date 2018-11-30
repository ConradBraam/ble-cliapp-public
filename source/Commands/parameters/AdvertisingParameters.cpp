//
// Created by Vincent Coubard on 2018-11-30.
//

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/BLECommonSerializer.h"
#include "CLICommand/CommandSuite.h"
#include "CLICommand/CommandHelper.h"

#include "AdvertisingParameters.h"

namespace {

// global parameters being modified
ble::AdvertisingParameters parameters;

DECLARE_CMD(Reset) {
    CMD_NAME("reset")
    CMD_HANDLER(CommandResponsePtr& response) {
        parameters = ble::AdvertisingParameters();
        response->success();
    }
};

DECLARE_CMD(SetType) {
    CMD_NAME("setType")
    CMD_HANDLER(ble::advertising_type_t::type type, CommandResponsePtr& response) {
        parameters.setType(type);
        response->success();
    }
};

DECLARE_CMD(SetPrimaryInterval) {
    CMD_NAME("setPrimaryInterval")
    CMD_HANDLER(ble::adv_interval_t min, ble::adv_interval_t max, CommandResponsePtr& response) {
        parameters.setPrimaryInterval(min, max);
        response->success();
    }
};

DECLARE_CMD(SetPrimaryChannels) {
    CMD_NAME("setPrimaryChannels")
    CMD_HANDLER(bool chan37, bool chan38, bool chan39, CommandResponsePtr& response) {
        parameters.setPrimaryChannels(chan37, chan38, chan39);
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

// TODO: set peer

DECLARE_CMD(SetFilter) {
    CMD_NAME("setFilter")
    CMD_HANDLER(ble::advertising_filter_policy_t::type filter, CommandResponsePtr& response) {
        parameters.setFilter(filter);
        response->success();
    }
};

DECLARE_CMD(SetPhy) {
    CMD_NAME("setPhy")
    CMD_HANDLER(ble::phy_t::type primaryPhy, ble::phy_t::type secondaryPhy, CommandResponsePtr& response) {
        parameters.setPhy(primaryPhy, secondaryPhy);
        response->success();
    }
};

DECLARE_CMD(SetTxPower) {
    CMD_NAME("setTxPower")
    CMD_HANDLER(ble::advertising_power_t power, CommandResponsePtr& response) {
        parameters.setTxPower(power);
        response->success();
    }
};

DECLARE_CMD(SetSecondaryMaxSkip) {
    CMD_NAME("setSecondaryMaxSkip")
    CMD_HANDLER(uint8_t eventNumber, CommandResponsePtr& response) {
        parameters.setSecondaryMaxSkip(eventNumber);
        response->success();
    }
};

DECLARE_CMD(SetScanRequestNotification) {
    CMD_NAME("setScanRequestNotification")
    CMD_HANDLER(bool enable, CommandResponsePtr& response) {
        parameters.setScanRequestNotification(enable);
        response->success();
    }
};

DECLARE_CMD(SetUseLegacyPDU) {
    CMD_NAME("setUseLegacyPDU")
    CMD_HANDLER(bool enable, CommandResponsePtr& response) {
        parameters.setUseLegacyPDU(enable);
        response->success();
    }
};

DECLARE_CMD(IncludeTxPowerInHeader) {
    CMD_NAME("IncludeTxPowerInHeader")
    CMD_HANDLER(bool enable, CommandResponsePtr& response) {
        parameters.includeTxPowerInHeader(enable);
        response->success();
    }
};

DECLARE_CMD(SetAnonymousAdvertising) {
    CMD_NAME("setAnonymousAdvertising")
    CMD_HANDLER(bool enable, CommandResponsePtr& response) {
        parameters.setAnonymousAdvertising(enable);
        response->success();
    }
};

}

DECLARE_SUITE_COMMANDS(AdvertisingParametersCommandSuiteDescription,
    CMD_INSTANCE(Reset),
    CMD_INSTANCE(SetType),
    CMD_INSTANCE(SetPrimaryInterval),
    CMD_INSTANCE(SetPrimaryChannels),
    CMD_INSTANCE(SetOwnAddressType),
    CMD_INSTANCE(SetFilter),
    CMD_INSTANCE(SetPhy),
    CMD_INSTANCE(SetTxPower),
    CMD_INSTANCE(SetSecondaryMaxSkip),
    CMD_INSTANCE(SetScanRequestNotification),
    CMD_INSTANCE(SetUseLegacyPDU),
    CMD_INSTANCE(IncludeTxPowerInHeader),
    CMD_INSTANCE(SetAnonymousAdvertising)
);