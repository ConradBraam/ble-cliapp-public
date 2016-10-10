#include <cstring>
#include <cctype>
#include <algorithm>

#include <ble/BLE.h>
#include <ble/SecurityManager.h>

#include "SecurityManagerCommands.h"
#include "Serialization/Serializer.h"
#include "Serialization/SecurityManagerSerialization.h"
#include "Serialization/BLECommonSerializer.h"
#include "Serialization/GapSerializer.h"

using mbed::util::SharedPointer;

// isolation
namespace {

static BLE& ble() {
    return BLE::Instance();
}

static SecurityManager& sm() {
    return ble().securityManager();
}

static void reportErrorOrSuccess(const SharedPointer<CommandResponse>& response, ble_error_t err) {
    if(err) {
        response->faillure(err);
    } else {
        response->success();
    }
}

template<typename T>
static void reportErrorOrSuccess(const SharedPointer<CommandResponse>& response, ble_error_t err, const T& res) {
    if(err) {
        response->faillure(err);
    } else {
        response->success(res);
    }
}

static bool is_digit(uint8_t v) {
    return (bool) std::isdigit(v);
}


struct InitCommand : public BaseCommand {
    static const char* name() {
        return "init";
    }

    static const char* help() {
        return "Enable the BLE stack's Security Manager.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<bool>", "enableBonding: Allow for bonding." },
            { "<bool>", "requireMITM   Require protection for man-in-the-middle attacks." },
            { "<SecurityManager::SecurityIOCapabilities_t>", "iocaps :To specify the I/O capabilities of this peripheral." },
            { "<Passkey_t>", "passkey: To specify a static passkey.." }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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


struct GetAddressesFromBondTableCommand : public BaseCommand {
    static const char* name() {
        return "getAddressesFromBondTable";
    }

    static const char* help() {
        return "Get a list of addresses from all peers in the bond table.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<uint8_t>", "addressesCount count of addresses to get" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        using namespace serialization;

        uint8_t addrCount;
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


struct PurgeAllBondingStateCommand : public BaseCommand {
    static const char* name() {
        return "purgeAllBondingState";
    }

    static const char* help() {
        return "Delete all peer device context and all related bonding information from "
        "the database within the security manager.";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = sm().purgeAllBondingState();
        reportErrorOrSuccess(response, err);
    }
};

} // end of annonymous namespace

ConstArray<const Command*> SecurityManagerCommandSuiteDescription::commands() {
    static const Command* commandHandlers[] = {
        &CommandAccessor<InitCommand>::command,
        &CommandAccessor<GetAddressesFromBondTableCommand>::command,
        &CommandAccessor<PurgeAllBondingStateCommand>::command
    };

    return ConstArray<const Command*>(commandHandlers);
}
