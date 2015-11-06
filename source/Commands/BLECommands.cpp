#include "BLECommands.h"
#include "util/StaticLambda.h"
#include "Serialization/Stringify.h"
   
// isolation
namespace {

static BLE& ble() {
    return BLE::Instance();
}

static constexpr const Command shutdown { 
    "shutdown",
    "Shutdown the current BLE instance, calling ble related function after this"  
    "call may lead to faillure.",
    STATIC_LAMBDA(const CommandArgs&) {
        ble_error_t err = ble().shutdown();
        return err ? CommandResult::faillure() : CommandResult::success();
    } 
};

static constexpr const Command init {
    "init",
    "Initialize the ble API and underlying BLE stack.\r\n"
    "Be sure to call this function before any other ble API function",
    STATIC_LAMBDA(const CommandArgs&) {
        if(ble().hasInitialized()) {
            return CommandResult::success();
        }

        ble().init([](BLE::InitializationCompleteCallbackContext* initializationStatus) {
            if(initializationStatus->error) {
                CommandSuite<BLECommandSuiteDescription>::commandReady(
                    init.name, 
                    CommandArgs(0, 0), 
                    CommandResult::faillure(to_string(initializationStatus->error))
                );
            } else {
                CommandSuite<BLECommandSuiteDescription>::commandReady(
                    init.name, 
                    CommandArgs(0, 0), 
                    CommandResult::success()
                );
            }
        });
        return CommandResult(CMDLINE_RETCODE_EXCUTING_CONTINUE);
    }
};


static constexpr const Command reset = {
    "reset",
    "Reset the ble API and ble stack.\r\n"
    "This function internaly does a reset and an init",
    STATIC_LAMBDA(const CommandArgs&) {
        ble_error_t err;
        if(ble().hasInitialized()) {
            err = ble().shutdown();
            if(err) {
                return CommandResult::faillure("Failled to shutdown the ble instance");
            }           
        }

        err = ble().init();
        if(err) {
            return CommandResult::faillure("Failled to init the ble instance");
        }

        return CommandResult::success();
    } 
};

static constexpr const Command getVersion {
    "getVersion",
    "Return the version of the BLE API.\r\n",
    STATIC_LAMBDA(const CommandArgs&) {
        const char* version = ble().getVersion();

        if(version) {
            return CommandResult::success(version);
        }

        return CommandResult::faillure("ble version is not available");
    } 
};

} // end of annonymous namespace

ConstArray<Command> BLECommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        shutdown, 
        init, 
        reset, 
        getVersion
    };

    return ConstArray<Command>(commandHandlers);        
}
