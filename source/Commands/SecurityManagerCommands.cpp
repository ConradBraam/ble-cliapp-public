#include <cstring>
#include <cctype>
#include <algorithm>

#include <ble/BLE.h>
#include <ble/SecurityManager.h>

#include "Serialization/Serializer.h"
#include "Serialization/SecurityManagerSerialization.h"
#include "Serialization/BLECommonSerializer.h"
#include "Serialization/GapSerializer.h"

#include "Common.h"

#include "SecurityManagerCommands.h"
#include "CLICommand/CommandHelper.h"
#include "CLICommand/util/AsyncProcedure.h"

using mbed::util::SharedPointer;

// isolation
namespace {

static bool is_digit(uint8_t v) {
    return (bool) std::isdigit(v);
}


DECLARE_CMD(InitCommand) {
    CMD_NAME("init")

    CMD_HELP("Enable the BLE stack's Security Manager.")

    CMD_ARGS(
        CMD_ARG("bool","enableBonding", "Allow bonding."),
        CMD_ARG("bool", "requireMITM", "Require protection for man-in-the-middle attacks."),
        CMD_ARG("SecurityManager::SecurityIOCapabilities_t", "iocaps", "Specify the I/O capabilities of this peripheral."),
        CMD_ARG("Passkey_t", "passkey", "Specify a static passkey."),
        CMD_ARG("bool", "signing", "Generate and distribute signing key during pairing."),
    )

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
        bool enableBonding;
        if(!fromString(args[0], enableBonding)) {
            response->invalidParameters("enableBonding should be a bool");
            return;
        }

        bool requireMITM;
        if(!fromString(args[1], requireMITM)) {
            response->invalidParameters("requireMITM should be a bool");
            return;
        }

        SecurityManager::SecurityIOCapabilities_t iocaps;
        if(!fromString(args[2], iocaps)) {
            response->invalidParameters("iocaps should be a SecurityManager::SecurityIOCapabilities_t");
            return;
        }

        SecurityManager::Passkey_t passkey;
        if(std::strlen(args[3]) != sizeof(passkey) ||
           std::count_if(args[3], args[3] + sizeof(passkey), is_digit) == sizeof(passkey)) {
            response->invalidParameters("passkey should be a SecurityManager::Passkey_t");
            return;
        }
        memcpy(passkey, args[3], sizeof(passkey));

        bool signing;
        if(!fromString(args[4], signing)) {
            response->invalidParameters("signing should be a bool");
            return;
        }

        ble_error_t err = sm().init(enableBonding, requireMITM, iocaps, passkey, signing);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(GetAddressesFromBondTableCommand) {
    CMD_NAME("getAddressesFromBondTable")

    CMD_HELP("Get a list of addresses from all peers in the bond table.")

    CMD_ARGS(
        CMD_ARG("uint8_t", "addresses_count", "count of addresses to get")
    )

    CMD_RESULTS(
        CMD_RESULT("JSON Array", "", "List of the addresses in the bond table."),
        CMD_RESULT("BLEProtocol::AddressType_t", "[].address_type", "Address type of the address."),
        CMD_RESULT("MacAddress", "[].address", "Mac address."),
    )

    CMD_HANDLER(uint8_t addrCount, CommandResponsePtr& response) {
        using namespace serialization;

        BLEProtocol::Address_t* addresses = new BLEProtocol::Address_t[addrCount];
        Gap::Whitelist_t whitelist = {
            addresses,
            0,
            addrCount
        };

        ble_error_t err = sm().getAddressesFromBondTable(whitelist);
        if(err) {
            response->faillure(err);
        } else {
            response->success();
            serialization::JSONOutputStream& os = response->getResultStream();

            os << startArray;
            for(std::size_t i = 0; i < whitelist.size; ++i) {
                os << startObject <<
                    key("address_type") << whitelist.addresses[i].type <<
                    key("address") << whitelist.addresses[i].address <<
                endObject;
            }
            os << endArray;
        }

        delete[] addresses;
    }
};

DECLARE_CMD(PreserveBondingStateOnResetCommand) {
    CMD_NAME("preserveBondingStateOnReset")

    CMD_ARGS(
        CMD_ARG("bool","enable", "enable if true the stack will attempt to preserve bonding information on reset.")
    )

    CMD_HELP("Normally all bonding information is lost when device is reset, this requests that the stack "
             "attempts to save the information and reload it during initialisation. This is not guaranteed.")

    CMD_HANDLER(bool enable, CommandResponsePtr& response) {
        ble_error_t err = sm().preserveBondingStateOnReset(enable);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(PurgeAllBondingStateCommand) {
    CMD_NAME("purgeAllBondingState")

    CMD_HELP("Delete all peer device context and all related bonding information from "
        "the database within the security manager.")

    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = sm().purgeAllBondingState();
        reportErrorOrSuccess(response, err);
    }
};

#define BLE_SM_TEST_ASSERT_RET(x, ret) do{ ble_error_t err = (x); if(err) { response->faillure(err); return ret; }  }while(0) 
#define BLE_SM_TEST_ASSERT_VOID(x) BLE_SM_TEST_ASSERT_RET(x, )

DECLARE_CMD(GenerateWhitelistFromBondTableCommand) {
    CMD_NAME("generateWhitelistFromBondTable")

    CMD_HELP("Create a list of addresses from all peers in the bond table and generate "
     " an event which returns it as a whitelist. Pass in the container for the whitelist. "
     " This will be returned by the event.")

    CMD_HANDLER(CommandResponsePtr& response) {
        startProcedure<GenerateWhitelistFromBondTableProcedure>(
            response, /* timeout */ 5 * 1000
        );
    }

    struct GenerateWhitelistFromBondTableProcedure : public AsyncProcedure, public SecurityManager::SecurityManagerEventHandler {
        GenerateWhitelistFromBondTableProcedure(const CommandResponsePtr& res, uint32_t timeout) 
            : AsyncProcedure(res, timeout) {
                // Initialize whitelist
                _whiteList.capacity = gap().getMaxWhitelistSize();

                void* addresses_ptr = malloc( sizeof(BLEProtocol::Address_t) * _whiteList.capacity );
                if(addresses_ptr != NULL) {
                    BLEProtocol::Address_t* addresses = new (addresses_ptr) BLEProtocol::Address_t[_whiteList.capacity];
                    _whiteList.addresses = addresses;
                }
                else
                {
                    _whiteList.addresses = NULL;
                    _whiteList.capacity = 0;
                }
                _whiteList.size = 0;

                // Set this struct as event handler
                sm().setSecurityManagerEventHandler(this);
            }

        virtual ~GenerateWhitelistFromBondTableProcedure() {
            // Deregister as event handler
            sm().setSecurityManagerEventHandler(NULL);

            // De-allocate addresses from whitelist
            for(size_t p = 0; p < _whiteList.capacity; p++)
            {
                _whiteList.addresses[p].~Address_t();
            }
            if(_whiteList.addresses != NULL) {
                free(_whiteList.addresses);
            }
        }

        virtual bool doStart() {
            if(_whiteList.addresses == NULL)
            {
                response->getResultStream() << "Could not allocate addresses table";
                response->faillure();
                return false;
            }

            BLE_SM_TEST_ASSERT_RET( sm().generateWhitelistFromBondTable(&_whiteList), false );
            return true;
        }

        virtual void doWhenTimeout() { 
            response->getResultStream() << "generateWhitelistFromBondTable timeout";
            response->faillure();
            terminate();
        }

        // SecurityManagerEventHandler implementation
        virtual void whitelistFromBondTable(Gap::Whitelist_t* whitelist) {
            using namespace serialization;

            // Print & exit with success
            response->success();
            serialization::JSONOutputStream& os = response->getResultStream();

            os << startArray;
            for(std::size_t i = 0; i < whitelist->size; ++i) {
                os << startObject <<
                    key("address_type") << whitelist->addresses[i].type <<
                    key("address") << whitelist->addresses[i].address <<
                endObject;
            }
            os << endArray;

            terminate();
        }

        // Data
        Gap::Whitelist_t _whiteList;
    };
};

// Pairing
// A pairing procedure can be started using the relevant command and can be continued afterwards
struct BasePairingProcedure : public AsyncProcedure, public SecurityManager::SecurityManagerEventHandler {
    BasePairingProcedure(uint16_t connectionHandle, const CommandResponsePtr& res, uint32_t timeout) 
        : AsyncProcedure(res, timeout), 
            _connectionHandle(connectionHandle)
        {
            // Set this struct as event handler
            sm().setSecurityManagerEventHandler(this);
        }

    virtual ~BasePairingProcedure() {
        // Deregister as event handler
        sm().setSecurityManagerEventHandler(NULL);
    }

    virtual bool doStart() {
        // Can be overriden if required
        return true;
    }

    virtual void doWhenTimeout() { 
        // Make sure we abort any ongoing pairing procedure
        sm().canceltPairingRequest(_connectionHandle);

        response->getResultStream() << "Pairing timeout";
        response->faillure();
    }

    void success(bool completed, const char* status, const SecurityManager::Passkey_t passkey = NULL)
    {
        using namespace serialization;

        response->success();

        JSONOutputStream& os = response->getResultStream();

        os << startObject <<
            key("completed") << completed <<
            key("status") << status;
            
        if(passkey != NULL) {
            os << key("passkey") << passkey;
        }

        os << endObject;

        terminate();
    }

    // SecurityManagerEventHandler implementation
    virtual void pairingRequest(connection_handle_t connectionHandle) {
        // Ignore if wrong connection handle
        if(connectionHandle != _connectionHandle) { return; }

        success(false, "pairingRequest");
    }

    virtual void pairingResult(connection_handle_t connectionHandle, SecurityManager::SecurityCompletionStatus_t result) {
        // Ignore if wrong connection handle
        if(connectionHandle != _connectionHandle) { return; }

        // Print & exit with success
        if(result == SecurityManager::SEC_STATUS_SUCCESS) {
            success(true, "completed");
            return;
        }
        else {
            response->getResultStream() << "pairingResult returned " << result;
            response->faillure();
            terminate();
        }
    }

    virtual void passkeyDisplay(connection_handle_t connectionHandle, const SecurityManager::Passkey_t passkey) {
        // Ignore if wrong connection handle
        if(connectionHandle != _connectionHandle) { return; }

        // Return passkey
        success(false, "passkeyDisplay", passkey);
    }

    virtual void confirmationRequest(connection_handle_t connectionHandle) {
        // Ignore if wrong connection handle
        if(connectionHandle != _connectionHandle) { return; }

        // Ask user to confirm or or reject
        success(false, "confirmationRequest");
    }

    virtual void passkeyRequest(connection_handle_t connectionHandle) {
        // Ignore if wrong connection handle
        if(connectionHandle != _connectionHandle) { return; }

        // Ask user to provide passkey
        success(false, "passkeyRequest");
    }

    // Data
    uint16_t _connectionHandle;
};

DECLARE_CMD(WaitForPairingCommand) {
    CMD_NAME("waitForPairing")

    CMD_HELP("This waits for and handles an incoming or ongoing pairing procedure. It waits for a request from peer or pairing completiomn.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure"),
        CMD_ARG("uint16_t", "timeout", "Time after which this command should fail")
    )

    CMD_HANDLER(uint16_t connectionHandle, uint16_t timeout, CommandResponsePtr& response) {
        startProcedure<BasePairingProcedure>(
            connectionHandle, 
            response, timeout
        );
    }
};

DECLARE_CMD(AcceptPairingRequestAndWaitCommand) {
    CMD_NAME("acceptPairingRequestAndWait")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "timeout", "Time after which this command should fail")
    )

    CMD_HELP("This waits for and handles an incoming or ongoing pairing procedure. It waits for a request from peer or pairing completiomn.")

    CMD_HANDLER(uint16_t connectionHandle, uint16_t timeout, CommandResponsePtr& response) {
        startProcedure<AcceptPairingRequestAndWaitProcedure>(
            connectionHandle, 
            response, timeout
        );
    }

    struct AcceptPairingRequestAndWaitProcedure : BasePairingProcedure
    {
        AcceptPairingRequestAndWaitProcedure(uint16_t connectionHandle, const CommandResponsePtr& res, uint32_t timeout) :
            BasePairingProcedure(connectionHandle, res, timeout)
        {

        }

        virtual bool doStart() {
            BLE_SM_TEST_ASSERT_RET(sm().acceptPairingRequest(_connectionHandle), false);
            return true;
        }
    };
};

DECLARE_CMD(RejectPairingRequestCommand) {
    CMD_NAME("rejectPairingRequest")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
    )

    CMD_HELP("This rejects an incoming pairing request.")

    CMD_HANDLER(uint16_t connectionHandle, CommandResponsePtr& response) {
        if(!sm().canceltPairingRequest(connectionHandle))
        {
            response->faillure();
            return;
        }
        response->success();
    }
};

DECLARE_CMD(EnterConfirmationAndWaitCommand) {
    CMD_NAME("enterConfirmationAndWait")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("bool", "confirm", "Whether to confirm the validity of the passkey" ),
        CMD_ARG("uint16_t", "timeout", "Time after which this command should fail")
    )

    CMD_HELP("This sends confirmation (yes or no) to the stack during pairing")

    CMD_HANDLER(uint16_t connectionHandle, bool confirm, uint16_t timeout, CommandResponsePtr& response) {
        startProcedure<EnterConfirmationAndWaitProcedure>(
            connectionHandle, confirm,
            response, timeout
        );
    }

    struct EnterConfirmationAndWaitProcedure : BasePairingProcedure
    {
        EnterConfirmationAndWaitProcedure(uint16_t connectionHandle, bool confirm, const CommandResponsePtr& res, uint32_t timeout) :
            BasePairingProcedure(connectionHandle, res, timeout), _confirm(confirm)
        {

        }

        virtual bool doStart() {
            BLE_SM_TEST_ASSERT_RET(sm().confirmationEntered(_connectionHandle, _confirm), false);

            return true;
        }

        bool _confirm;
    };
};

DECLARE_CMD(EnterPasskeyAndWaitCommand) {
    CMD_NAME("enterPasskeyAndWait")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("SecurityManagerPasskey_t","passkey", "Numeric passkey to use during pairing if asked for check (this is what the user would consider the passkey to be - this passkey can be set to something unexpected if required to simulate error cases)."),        // todo add oob
        CMD_ARG("uint16_t", "timeout", "Time after which this command should fail")
    )

    CMD_HELP("This sends confirmation (yes or no) to the stack during pairing")

    CMD_HANDLER(uint16_t connectionHandle, const SecurityManagerPasskey_t passkey, uint16_t timeout, CommandResponsePtr& response) {
        startProcedure<EnterPasskeyAndWaitProcedure>(
            connectionHandle, passkey,
            response, timeout
        );
    }

    struct EnterPasskeyAndWaitProcedure : BasePairingProcedure
    {
        EnterPasskeyAndWaitProcedure(uint16_t connectionHandle, const SecurityManagerPasskey_t& passkey, const CommandResponsePtr& res, uint32_t timeout) :
            BasePairingProcedure(connectionHandle, res, timeout)
        {
            memcpy(_passkey, passkey.value, sizeof(SecurityManager::Passkey_t));
        }

        virtual bool doStart() {
            // Provide passkey
            BLE_SM_TEST_ASSERT_RET(sm().passkeyEntered(_connectionHandle, _passkey), false);

            return true;
        }

        SecurityManager::Passkey_t _passkey;
    };
};

DECLARE_CMD(RequestPairingAndWaitCommand) {
    CMD_NAME("requestPairingAndWait")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "pairing_timeout", "Time after which the authentication should fail"),
        CMD_ARG("uint16_t", "timeout", "Time after which this command should fail")
    )

    CMD_HELP("This performs a pairing procedure when the device acts as an initiator.")

    CMD_HANDLER(uint16_t connectionHandle, uint16_t pairing_timeout, uint16_t timeout, CommandResponsePtr& response) {
        startProcedure<RequestPairingAndWaitProcedure>(
            connectionHandle, pairing_timeout,
            response, timeout
        );
    }

    struct RequestPairingAndWaitProcedure : BasePairingProcedure
    {
        RequestPairingAndWaitProcedure(uint16_t connectionHandle, uint16_t pairing_timeout, const CommandResponsePtr& res, uint32_t timeout) :
            BasePairingProcedure(connectionHandle, res, timeout), _pairing_timeout(pairing_timeout)
        {

        }

        virtual bool doStart() {
            BLE_SM_TEST_ASSERT_RET(sm().requestPairing(_connectionHandle), false);
            return true;
        }

        uint16_t _pairing_timeout;
    };
};

DECLARE_CMD(AllowLegacyPairingCommand) {
    CMD_NAME("allowLegacyPairing")

    CMD_ARGS(
        CMD_ARG("bool", "allow", "if true, legacy pairing will be used if either peer doesn't support Secure Connections.")
    )

    CMD_HELP("Allow of disallow the use of legacy pairing in case the application only wants "
     "to force the use of Secure Connections. If legacy pairing is disallowed and either "
     "side doesn't support Secure Connections the pairing will fail.")

    CMD_HANDLER(bool allow, CommandResponsePtr& response) {
        ble_error_t err = sm().allowLegacyPairing(allow);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(GetSecureConnectionsSupportCommand) {
    CMD_NAME("getSecureConnectionsSupport")
    
    CMD_HELP("Check if the Secure Connections feature is supported by the stack and controller.")

    CMD_HANDLER(CommandResponsePtr& response) {
        bool enabled = false;
        ble_error_t err = sm().getSecureConnectionsSupport(&enabled);
        reportErrorOrSuccess(response, err, enabled);
    }
};

DECLARE_CMD(SetIoCapabilityCommand) {
    CMD_NAME("setIoCapability")

    CMD_ARGS(
        CMD_ARG("SecurityManager::SecurityIOCapabilities_t", "iocaps", "type of IO capabilities available on the local device"),
    )

    CMD_HELP("Set the IO capability of the local device.")

    CMD_HANDLER(SecurityManager::SecurityIOCapabilities_t iocaps, CommandResponsePtr& response) {
        ble_error_t err = sm().setIoCapability(iocaps);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(SetDisplayPasskeyCommand) {
    CMD_NAME("setDisplayPasskey")

    CMD_ARGS(
        CMD_ARG("SecurityManagerPasskey_t", "passkey", "Numeric passkey to use during pairing if asked for check (this is what the user would consider the passkey to be - this passkey can be set to something unexpected if required to simulate error cases)."),        // todo add oob
    )

    CMD_HELP("Set the passkey that is displayed on the local device instead of using "
             "a randomly generated one")

    CMD_HANDLER(const SecurityManagerPasskey_t passkey, CommandResponsePtr& response) {
        ble_error_t err = sm().setDisplayPasskey(passkey);
        reportErrorOrSuccess(response, err);
    }
};

} // end of anonymous namespace


DECLARE_SUITE_COMMANDS(SecurityManagerCommandSuiteDescription, 
    CMD_INSTANCE(InitCommand),
    CMD_INSTANCE(GetAddressesFromBondTableCommand),
    CMD_INSTANCE(PreserveBondingStateOnResetCommand),
    CMD_INSTANCE(PurgeAllBondingStateCommand),
    CMD_INSTANCE(GenerateWhitelistFromBondTableCommand),
    
    // Pairing commands
    CMD_INSTANCE(WaitForPairingCommand),
    CMD_INSTANCE(AcceptPairingRequestAndWaitCommand),
    CMD_INSTANCE(RejectPairingRequestCommand),
    CMD_INSTANCE(EnterConfirmationAndWaitCommand),
    CMD_INSTANCE(EnterPasskeyAndWaitCommand),
    CMD_INSTANCE(RequestPairingAndWaitCommand),

    // Configuration commands
    CMD_INSTANCE(AllowLegacyPairingCommand),
    CMD_INSTANCE(GetSecureConnectionsSupportCommand),
    CMD_INSTANCE(SetIoCapabilityCommand),
    CMD_INSTANCE(SetDisplayPasskeyCommand)

)