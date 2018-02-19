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

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
        bool enable;
        if(!fromString(args[0], enable)) {
            response->invalidParameters("enable should be a bool");
            return;
        }

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
                BLEProtocol::Address_t* addresses = new BLEProtocol::Address_t[gap().getMaxWhitelistSize()]();
                whiteList.addresses = addresses;
                whiteList.size = 0;
                whiteList.capacity = gap().getMaxWhitelistSize();

                // Set this struct as event handler
                sm().setSecurityManagerEventHandler(this);
            }

        virtual ~GenerateWhitelistFromBondTableProcedure() {
            // Deregister as event handler
            sm().setSecurityManagerEventHandler(NULL);
        }

        virtual bool doStart() {
            BLE_SM_TEST_ASSERT_RET( sm().generateWhitelistFromBondTable(&whiteList), false );
            return true;
        }

        virtual void doWhenTimeout() { 
            response->getResultStream() << "generateWhitelistFromBondTable timeout";
            response->faillure();
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

        Gap::Whitelist_t whiteList;
    };
};

DECLARE_CMD(HandlePairingCommand) {
    CMD_NAME("handlePairing")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("bool", "accept", "If true, accept request, if not reject it."),
        CMD_ARG("SecurityManagerPasskey_t","passkey", "Numeric passkey to use during pairing if asked for check."),
        // todo add oob
        CMD_ARG("uint16_t", "timeout", "Time after which the authentication should fail")
    )

    CMD_HELP("This waits for and handles an incoming pairing procedure. It waits for a request from peer.")

    CMD_HANDLER(uint16_t connectionHandle, bool accept, SecurityManagerPasskey_t passkey, uint16_t pairing_timeout, CommandResponsePtr& response) {
        startProcedure<HandlePairingProcedure>(
            connectionHandle, accept, passkey, pairing_timeout,
            response, /* timeout */ 5 * 1000
        );
    }

    struct HandlePairingProcedure : public AsyncProcedure, public SecurityManager::SecurityManagerEventHandler {
        HandlePairingProcedure(uint16_t connectionHandle, bool accept, const SecurityManager::Passkey_t passkey, uint16_t pairing_timeout,
                                const CommandResponsePtr& res, uint32_t timeout) 
            : AsyncProcedure(res, timeout), 
                _connectionHandle(connectionHandle),
                _accept(accept),
                _pairing_timeout(pairing_timeout)
            {
                memset(_peer_passkey, 0xff /* that's invalid on purpose */, sizeof(SecurityManager::Passkey_t));
                memcpy(_our_passkey, passkey, sizeof(SecurityManager::Passkey_t));

                // Set this struct as event handler
                sm().setSecurityManagerEventHandler(this);
            }

        virtual ~HandlePairingProcedure() {
            // Deregister as event handler
            sm().setSecurityManagerEventHandler(NULL);
        }

        virtual bool doStart() {
            // Wait... for request

            return true;
        }

        virtual void doWhenTimeout() { 
            response->getResultStream() << "generateWhitelistFromBondTable timeout";
            response->faillure();
        }

        // SecurityManagerEventHandler implementation
        virtual void pairingRequest(connection_handle_t connectionHandle) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            if(_accept)
            {
                BLE_SM_TEST_ASSERT_VOID(sm().acceptPairingRequest(connectionHandle));
            }
            else
            {
                BLE_SM_TEST_ASSERT_VOID(sm().canceltPairingRequest(connectionHandle));
            }
        }

        virtual void pairingResult(connection_handle_t connectionHandle, SecurityManager::SecurityCompletionStatus_t result) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // FIXME what if rejecting?

            // Print & exit with success
            if(result == SecurityManager::SEC_STATUS_SUCCESS) {
                response->success();
                terminate();
            }
            else {
                response->getResultStream() << "pairingResult returned " << result;
                response->faillure();
            }
        }

        virtual void passkeyDisplay(connection_handle_t connectionHandle, const SecurityManager::Passkey_t passkey) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // Save passkey
            memcpy(_peer_passkey, passkey, sizeof(SecurityManager::Passkey_t));
        }

        virtual void confirmationRequest(connection_handle_t connectionHandle) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // Confirm or ignore based on whether passkeys match
            if(!memcmp(_our_passkey, _peer_passkey, sizeof(SecurityManager::Passkey_t)))
            {
                // OK
                BLE_SM_TEST_ASSERT_VOID(sm().confirmationEntered(_connectionHandle, true));
            }
            else
            {
                // No sugar
                BLE_SM_TEST_ASSERT_VOID(sm().confirmationEntered(_connectionHandle, false));
            }
        }

        virtual void passkeyRequest(connection_handle_t connectionHandle) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // Provide passkey
            BLE_SM_TEST_ASSERT_VOID(sm().passkeyEntered(connectionHandle, _our_passkey));
        }


        // Data
        uint16_t _connectionHandle;
        bool _accept;
        SecurityManager::Passkey_t _our_passkey;
        SecurityManager::Passkey_t _peer_passkey;
        uint16_t _pairing_timeout;
    };
};

DECLARE_CMD(ExecutePairingCommand) {
    CMD_NAME("executePairing")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("SecurityManagerPasskey_t","passkey", "Numeric passkey to use during pairing if asked for check."),
        // todo add oob
        CMD_ARG("uint16_t", "timeout", "Time after which the authentication should fail")
    )

    CMD_HELP("This performs a pairing procedure when the device acts as an initiator.")

    CMD_HANDLER(uint16_t connectionHandle, SecurityManagerPasskey_t passkey, uint16_t pairing_timeout, CommandResponsePtr& response) {
        startProcedure<ExecutePairingProcedure>(
            connectionHandle, passkey, pairing_timeout,
            response, /* timeout */ 5 * 1000
        );
    }

    struct ExecutePairingProcedure : public AsyncProcedure, public SecurityManager::SecurityManagerEventHandler {
        ExecutePairingProcedure(uint16_t connectionHandle, const SecurityManager::Passkey_t passkey, uint16_t pairing_timeout,
                                const CommandResponsePtr& res, uint32_t timeout) 
            : AsyncProcedure(res, timeout),
                _connectionHandle(connectionHandle),
                _pairing_timeout(pairing_timeout)
            {
                memset(_peer_passkey, 0xff /* that's invalid on purpose */, sizeof(SecurityManager::Passkey_t));
                memcpy(_our_passkey, passkey, sizeof(SecurityManager::Passkey_t));

                // Set this struct as event handler
                sm().setSecurityManagerEventHandler(this);
            }

        virtual ~ExecutePairingProcedure() {
            // Deregister as event handler
            sm().setSecurityManagerEventHandler(NULL);
        }

        virtual bool doStart() {
            BLE_SM_TEST_ASSERT_RET(sm().requestPairing(_connectionHandle), false);
            return true;
        }

        virtual void doWhenTimeout() { 
            response->getResultStream() << "generateWhitelistFromBondTable timeout";
            response->faillure();
        }

        // SecurityManagerEventHandler implementation
        virtual void pairingResult(connection_handle_t connectionHandle, SecurityManager::SecurityCompletionStatus_t result) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // FIXME what if rejecting?

            // Print & exit with success
            if(result == SecurityManager::SEC_STATUS_SUCCESS) {
                response->success();
                terminate();
            }
            else {
                response->getResultStream() << "pairingResult returned " << result;
                response->faillure();
            }
        }

        virtual void passkeyDisplay(connection_handle_t connectionHandle, const SecurityManager::Passkey_t passkey) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // Save passkey
            memcpy(_peer_passkey, passkey, sizeof(SecurityManager::Passkey_t));
        }

        virtual void confirmationRequest(connection_handle_t connectionHandle) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // Confirm or ignore based on whether passkeys match
            if(!memcmp(_our_passkey, _peer_passkey, sizeof(SecurityManager::Passkey_t)))
            {
                // OK
                BLE_SM_TEST_ASSERT_VOID(sm().confirmationEntered(_connectionHandle, true));
            }
            else
            {
                // No sugar
                BLE_SM_TEST_ASSERT_VOID(sm().confirmationEntered(_connectionHandle, false));
            }
        }

        virtual void passkeyRequest(connection_handle_t connectionHandle) {
            // Ignore if wrong connection handle
            if(connectionHandle != _connectionHandle) { return; }

            // Provide passkey
            BLE_SM_TEST_ASSERT_VOID(sm().passkeyEntered(connectionHandle, _our_passkey));
        }

        // Data
        uint16_t _connectionHandle;
        SecurityManager::Passkey_t _our_passkey;
        SecurityManager::Passkey_t _peer_passkey;
        uint16_t _pairing_timeout;
    };
};


} // end of annonymous namespace


DECLARE_SUITE_COMMANDS(SecurityManagerCommandSuiteDescription, 
    CMD_INSTANCE(InitCommand),
    CMD_INSTANCE(GetAddressesFromBondTableCommand),
    CMD_INSTANCE(PreserveBondingStateOnResetCommand),
    CMD_INSTANCE(PurgeAllBondingStateCommand),
    CMD_INSTANCE(GenerateWhitelistFromBondTableCommand),
    CMD_INSTANCE(HandlePairingCommand),
    CMD_INSTANCE(ExecutePairingCommand)
)