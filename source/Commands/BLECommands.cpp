#include "BLECommands.h"
#include "Serialization/BLECommonSerializer.h"
#include "ble/FunctionPointerWithContext.h"
#include "CLICommand/util/AsyncProcedure.h"
#include "CLICommand/CommandHelper.h"
#include "Common.h"

#if defined(NO_FILESYSTEM)
#include "LittleFileSystem.h"
#include "HeapBlockDevice.h"
#endif //defined(NO_FILESYSTEM)

using mbed::util::SharedPointer;

// isolation
namespace {

DECLARE_CMD(ShutdownCommand) {
    CMD_NAME("shutdown")
    
    CMD_HELP(
        "Shutdown the current BLE instance, calling ble related function after this"
        "call may lead to faillure."
    )
    
    CMD_HANDLER(CommandResponsePtr& response) { 
        ble_error_t err = get_ble().shutdown();
        if(err) {
            response->faillure(err);
        } else {
            response->success();
        }
    }
};

DECLARE_CMD(InitCommand) {
    CMD_NAME("init")

    CMD_HELP(
        "Initialize the ble API and underlying BLE stack.\r\n"
        "Be sure to call this function before any other ble API function"
    )
    
    CMD_HANDLER(CommandResponsePtr& response) { 
        if(get_ble().hasInitialized()) {
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
            get_ble().init(this, &InitProcedure::whenInit);
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


DECLARE_CMD(ResetCommand) {
    CMD_NAME("reset")
    
    CMD_HELP(
        "Reset the ble API and ble stack."
        "This function internaly does a reset and an init"
    )
    
    CMD_HANDLER(CommandResponsePtr& response) { 
        ble_error_t err;
        if(get_ble().hasInitialized()) {
            err = get_ble().shutdown();
            if(err) {
                response->faillure("Failled to shutdown the ble instance");
                return;
            }
        }

        err = get_ble().init();
        if(err) {
            response->faillure("Failled to init the ble instance");
        } else {
            response->success();
        }
    }
};


DECLARE_CMD(GetVersionCommand) {
    CMD_NAME("getVersion")
    
    CMD_HELP("Return the version of the BLE API.")
    
    CMD_RESULTS(
        CMD_RESULT("string", "", "The version of the stack used by BLE API.")
    )

    CMD_HANDLER(CommandResponsePtr& response) { 
        const char* version = get_ble().getVersion();

        if(version) {
            response->success(version);
        } else {
            response->faillure("ble version is not available");
        }
    }
};


DECLARE_CMD(CreateFilesystem) {
    CMD_NAME("createFilesystem")

    CMD_HELP("Create a filesystem")

    CMD_HANDLER(CommandResponsePtr& response) {
#if defined(NO_FILESYSTEM)
        static LittleFileSystem& fs = *(new LittleFileSystem("fs"));
        static HeapBlockDevice& bd = *(new HeapBlockDevice(4096, 256));

        bd.init();
        bd.erase(0, bd.size());
        int err = fs.mount(&bd);
        if (err) {
            err = fs.reformat(&bd);
        }

        if (err == 0) {
            response->success();
        } else {
            response->faillure();
        }
#else
        response->faillure();
#endif //defined(NO_FILESYSTEM)
    }
};

} // end of annonymous namespace


DECLARE_SUITE_COMMANDS(BLECommandSuiteDescription, 
    CMD_INSTANCE(ShutdownCommand),
    CMD_INSTANCE(InitCommand),
    CMD_INSTANCE(ResetCommand),
    CMD_INSTANCE(GetVersionCommand),
    CMD_INSTANCE(CreateFilesystem)
)
