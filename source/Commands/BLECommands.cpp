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


struct ShutdownCommand : public Command {
    virtual const char* name() const {
        return "shutdown";
    }

    virtual const char* help() const {
        return "Shutdown the current BLE instance, calling ble related function after this"
               "call may lead to faillure.";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        ble_error_t err = ble().shutdown();
        if(err) {
            response->faillure(err);
        } else {
            response->success();
        }
    }
};


struct InitCommand : public Command {
    virtual const char* name() const {
        return "init";
    }

    virtual const char* help() const {
        return "Initialize the ble API and underlying BLE stack.\r\n"
        "Be sure to call this function before any other ble API function";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
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


struct ResetCommand : public Command {
    virtual const char* name() const {
        return "reset";
    }

    virtual const char* help() const {
        return "Reset the ble API and ble stack.\r\n"
               "This function internaly does a reset and an init";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
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


struct GetVersionCommand : public Command {
    virtual const char* name() const {
        return "getVersion";
    }

    virtual const char* help() const {
        return "Return the version of the BLE API.\r\n";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        const char* version = ble().getVersion();

        if(version) {
            response->success(version);
        } else {
            response->faillure("ble version is not available");
        }
    }
};

} // end of annonymous namespace

ConstArray<CommandAccessor_t> BLECommandSuiteDescription::commands() {
    static const CommandAccessor_t commandHandlers[] = {
        &getCommand<ShutdownCommand>,
        &getCommand<InitCommand>,
        &getCommand<ResetCommand>,
        &getCommand<GetVersionCommand>
    };

    return ConstArray<CommandAccessor_t>(commandHandlers);
}
