#include "BLECommands.h"
#include "Serialization/BLECommonSerializer.h"
#include "ble/FunctionPointerWithContext.h"
#include "util/AsyncProcedure.h"

using mbed::util::SharedPointer;

// isolation
namespace {

static BLE& ble() {
    return BLE::Instance();
}


struct ShutdownCommand : public BaseCommand {
    static const char* name() {
        return "shutdown";
    }

    static const char* help() {
        return "Shutdown the current BLE instance, calling ble related function after this"
               "call may lead to faillure.";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = ble().shutdown();
        if(err) {
            response->faillure(err);
        } else {
            response->success();
        }
    }
};


struct InitCommand : public BaseCommand {
    static const char* name() {
        return "init";
    }

    static const char* help() {
        return "Initialize the ble API and underlying BLE stack.\r\n"
        "Be sure to call this function before any other ble API function";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        if(ble().hasInitialized()) {
            response->success();
            return;
        }

        startProcedure<InitProcedure>(response, /* timeout */ 100 * 1000);
    }

    struct InitProcedure : public AsyncProcedure {
        InitProcedure(const SharedPointer<CommandResponse>& res, uint32_t procedureTimeout) :
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
};


struct ResetCommand : public BaseCommand {
    static const char* name() {
        return "reset";
    }

    static const char* help() {
        return "Reset the ble API and ble stack.\r\n"
               "This function internaly does a reset and an init";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
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


struct GetVersionCommand : public BaseCommand {
    static const char* name() {
        return "getVersion";
    }

    static const char* help() {
        return "Return the version of the BLE API.\r\n";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        const char* version = ble().getVersion();

        if(version) {
            response->success(version);
        } else {
            response->faillure("ble version is not available");
        }
    }
};

} // end of annonymous namespace

ConstArray<const Command*> BLECommandSuiteDescription::commands() {
    static const Command* commandHandlers[] = {
        &CommandAccessor<ShutdownCommand>::command,
        &CommandAccessor<InitCommand>::command,
        &CommandAccessor<ResetCommand>::command,
        &CommandAccessor<GetVersionCommand>::command
    };

    return ConstArray<const Command*>(commandHandlers);
}
