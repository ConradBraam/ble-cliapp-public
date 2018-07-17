#ifndef BLE_CLIAPP_GAP_COMMANDS_H_
#define BLE_CLIAPP_GAP_COMMANDS_H_

#include "CLICommand/CommandSuite.h"

struct GapEventHandler : Gap::EventHandler {
    static void RegisterEventHandler();

    virtual void onReadPhy(
        ble_error_t status,
        ble::connection_handle_t connectionHandle,
        ble::phy_t txPhy,
        ble::phy_t rxPhy
    );

    virtual void onPhyUpdateComplete(
        ble_error_t status,
        ble::connection_handle_t connectionHandle,
        ble::phy_t txPhy,
        ble::phy_t rxPhy
    );
};

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
