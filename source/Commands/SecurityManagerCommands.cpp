#include <cstring>
#include <cctype>
#include <algorithm>

#include <ble/BLE.h>
#include <ble/SecurityManager.h>

#include "SecurityManagerCommands.h"
#include "util/StaticLambda.h"
#include "Serialization/Serializer.h"
#include "Serialization/SecurityManagerSerialization.h"
#include "Serialization/BLECommonSerializer.h"
#include "Serialization/GapSerializer.h"

template<typename T>
using SharedPointer = mbed::util::SharedPointer<T>;

// isolation
namespace {

static BLE& ble() {
    return BLE::Instance();
}

static SecurityManager& sm() {
    return ble().securityManager();
}

static constexpr const Command init {
    "init",
    "Enable the BLE stack's Security Manager.",
    (const CommandArgDescription[]) {
        { "<bool>", "enableBonding: Allow for bonding." },
        { "<bool>", "requireMITM   Require protection for man-in-the-middle attacks." },
        { "<SecurityManager::SecurityIOCapabilities_t>", "iocaps :To specify the I/O capabilities of this peripheral." },
        { "<Passkey_t>", "passkey: To specify a static passkey.." }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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
           std::count_if(args[3], args[3] + sizeof(passkey), [](std::uint8_t v) { return (bool) std::isdigit(v); }) == sizeof(passkey)) {
            response->invalidParameters("passkey should be a SecurityManager::Passkey_t");
            return;
        }
        memcpy(passkey, args[3], sizeof(passkey));


        ble_error_t err = sm().init(enableBonding, requireMITM, iocaps, passkey);
        if(err) {
            response->faillure(err);
        } else {
            response->success();
        }
    }
};

static constexpr const Command getAddressesFromBondTable {
    "getAddressesFromBondTable",
    "Get a list of addresses from all peers in the bond table.",
    (const CommandArgDescription[]) {
        { "<uint8_t>", "addressesCount count of addresses to get" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        using namespace serialization;

        std::uint8_t addrCount;
        if(!fromString(args[0], addrCount)) {
            response->invalidParameters("address count should be a value in STD::uint_t domain");
            return;
        }

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
            auto& os = response->getResultStream();

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

} // end of annonymous namespace

ConstArray<Command> SecurityManagerCommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        init,
        getAddressesFromBondTable
    };

    return ConstArray<Command>(commandHandlers);
}
