#include "BLECommands.h"
#include "util/StaticLambda.h"
#include "Serialization/BLECommonSerializer.h"
#include "ble/FunctionPointerWithContext.h"
#include "util/AsyncProcedure.h"

// isolation
namespace {

static BLE& ble() {
    return BLE::Instance();
}

static constexpr const Command shutdown {
    "shutdown",
    "Shutdown the current BLE instance, calling ble related function after this"
    "call may lead to faillure.",
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        ble_error_t err = ble().shutdown();
        if(err) {
            response->faillure(err);
        } else {
            response->success();
        }
    }
};

static constexpr const Command init {
    "init",
    "Initialize the ble API and underlying BLE stack.\r\n"
    "Be sure to call this function before any other ble API function",
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        if(ble().hasInitialized()) {
            response->success();
            return;
        }

        struct InitProcedure : public AsyncProcedure {
            InitProcedure(const std::shared_ptr<CommandResponse>& res, uint32_t procedureTimeout) :
                AsyncProcedure(res, procedureTimeout) {
            }

            virtual bool doStart() {
                ble().init(this, &InitProcedure::whenInit);
                return true;
            }

            void whenInit(BLE::InitializationCompleteCallbackContext* initializationStatus) {
                if(initializationStatus->error) {
                    response->faillure(initializationStatus->error);
                } else {
                    response->success();
                }
                terminate();
            }
        };

        startProcedure<InitProcedure>(response, /* timeout */ 100 * 1000);
    }
};

static constexpr const Command reset = {
    "reset",
    "Reset the ble API and ble stack.\r\n"
    "This function internaly does a reset and an init",
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        ble_error_t err;
        if(ble().hasInitialized()) {
            err = ble().shutdown();
            if(err) {
                response->faillure("Failled to shutdown the ble instance");
                return;
            }
        }

        err = ble().init();
        if(err) {
            response->faillure("Failled to init the ble instance");
        } else {
            response->success();
        }
    }
};

static constexpr const Command getVersion {
    "getVersion",
    "Return the version of the BLE API.\r\n",
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        const char* version = ble().getVersion();

        if(version) {
            response->success(version);
        } else {
            response->faillure("ble version is not available");
        }
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
