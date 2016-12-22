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
        CMD_ARG("Passkey_t", "passkey", "Specify a static passkey.")
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


        ble_error_t err = sm().init(enableBonding, requireMITM, iocaps, passkey);
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


DECLARE_CMD(PurgeAllBondingStateCommand) {
    CMD_NAME("purgeAllBondingState")

    CMD_HELP("Delete all peer device context and all related bonding information from "
        "the database within the security manager.")

    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = sm().purgeAllBondingState();
        reportErrorOrSuccess(response, err);
    }
};

} // end of annonymous namespace


DECLARE_SUITE_COMMANDS(SecurityManagerCommandSuiteDescription, 
    CMD_INSTANCE(InitCommand),
    CMD_INSTANCE(GetAddressesFromBondTableCommand),
    CMD_INSTANCE(PurgeAllBondingStateCommand)
)