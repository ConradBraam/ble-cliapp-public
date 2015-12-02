#include "GattClientCommands.h"
#include "util/StaticLambda.h"
#include "ble/BLE.h"
#include "Serialization/Serializer.h"

// TODO: description of returned results

   
// isolation
namespace {
static BLE& ble() {
    return BLE::Instance();
}

static constexpr const Command discoverAllServicesAndCharacteristics { 
    "discoverAllServicesAndCharacteristics",
    "discover all services and characteristics available on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by the discovery procedure" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    } 
};

static constexpr const Command discoverAllServices { 
    "discoverAllServices",
    "discover all services available on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by the discovery procedure" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    } 
};

static constexpr const Command discoverServices { 
    "discoverServices",
    "discover a specific kind of services on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by the discovery procedure" },
        { "<serviceUUID>", "The UUID of the services to discover" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    } 
};



} // end of annonymous namespace

ConstArray<Command> GattClientCommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        discoverAllServicesAndCharacteristics,
        discoverAllServices,
        discoverServices
    };

    return ConstArray<Command>(commandHandlers);        
}
