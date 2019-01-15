

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/GapAdvertisingDataSerializer.h"
#include "Serialization/GapAdvertisingParamsSerializer.h"
#include "Serialization/BLECommonSerializer.h"
#include "CLICommand/CommandSuite.h"
#include "CLICommand/util/AsyncProcedure.h"
#include "Common.h"
#include "CLICommand/CommandHelper.h"

#ifdef YOTTA_CFG
#include "mbed-drivers/Timer.h"
#else
#include "Timer.h"
#endif

#include "GapCommands.h"
#include "GapV1Commands.h"
#include "GapV2Commands.h"

using mbed::util::SharedPointer;

typedef BLEProtocol::AddressType_t LegacyAddressType_t;

// isolation ...
namespace {

DECLARE_CMD(SetAddressCommand) {
    CMD_NAME("setAddress")
    CMD_HELP("set the address of this device")
    CMD_ARGS(
        CMD_ARG("BLEProtocol::AddressType_t", "addressType", "The type of the address to set."),
        CMD_ARG("MacAddress_t", "address" , "The address to set")
    )
    CMD_HANDLER(LegacyAddressType_t addressType, MacAddress_t address, CommandResponsePtr& response) {
        ble_error_t err = gap().setAddress(addressType, address);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(GetAddressCommand) {
    CMD_NAME("getAddress")
    CMD_HELP(
        "Get the address and the type of address of this device."
    )

    CMD_RESULTS(
        CMD_RESULT("BLEProtocol::AddressType_t", "address_type", "Type of the address"),
        CMD_RESULT("MacAddress_t", "address", "The mac address of the device")
    )

    CMD_HANDLER(CommandResponsePtr& response) {
        using namespace serialization;

        BLEProtocol::AddressType_t addressType;
        Gap::Address_t address;

        ble_error_t err = gap().getAddress(&addressType, address);
        if(err) {
            response->faillure(err);
            return;
        }

        // building the result object
        response->success();
        response->getResultStream() << startObject <<
            key("address_type") << addressType <<
            key("address") << address <<
        endObject;
    }
};


DECLARE_CMD(GetMinAdvertisingIntervalCommand) {
    CMD_NAME("getMinAdvertisingInterval")
    CMD_HELP("Return the minimum advertising interval")
    CMD_RESULTS(
        CMD_RESULT("uint16_t", "", "The minimum advertising interval.")
    )
    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getMinNonConnectableAdvertisingInterval());
    }
};


DECLARE_CMD(GetMinNonConnectableAdvertisingIntervalCommand) {
    CMD_NAME("getMinNonConnectableAdvertisingInterval")
    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getMinNonConnectableAdvertisingInterval());
    }
};


DECLARE_CMD(GetMaxAdvertisingIntervalCommand) {
    CMD_NAME("getMaxAdvertisingInterval")
    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getMaxAdvertisingInterval());
    }
};


DECLARE_CMD(SetDeviceNameCommand) {
    CMD_NAME("setDeviceName")
    CMD_HELP("set the device name")

    CMD_ARGS(
        CMD_ARG("string", "name", "the name of the device, it should not have space")
    )

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
        ble_error_t err = gap().setDeviceName((const uint8_t*) args[0]);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(GetDeviceNameCommand) {
    CMD_NAME("getDeviceName")
    CMD_HELP("return the device name as a string")

    CMD_HANDLER(CommandResponsePtr& response) {
        // first : collect the size of the name
        unsigned deviceNameLength = 0;
        ble_error_t err = gap().getDeviceName(NULL, &deviceNameLength);

        if(err) {
            response->faillure("impossible to get the length of the name");
            return;
        }

        // acquire the name
        ++deviceNameLength;  // add null terminator to the length
        uint8_t* deviceName = static_cast<uint8_t*>(std::malloc(deviceNameLength));
        memset(deviceName, 0, deviceNameLength);
        err = gap().getDeviceName(deviceName, &deviceNameLength);

        if(err) {
            std::free(deviceName);
            response->faillure(err);
            return;
        }

        response->success((const char*) deviceName);
        std::free(deviceName);
    }
};


DECLARE_CMD(SetAppearanceCommand) {
    CMD_NAME("setAppearance")
    CMD_HELP("set the appearance flag of the device")

    CMD_ARGS(
        CMD_ARG("GapAdvertisingData::Appearance_t", "appearance", "the appearance to set (see GapAdvertisingData::Appearance_t)")
    )

    CMD_HANDLER(GapAdvertisingData::Appearance_t appearance, CommandResponsePtr& response) {
        ble_error_t err = gap().setAppearance(appearance);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(GetAppearanceCommand) {
    CMD_NAME("getAppearance")
    CMD_HELP("get the appearance of the device")

    CMD_HANDLER(CommandResponsePtr& response) {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        ble_error_t err = gap().getAppearance(&appearance);
        reportErrorOrSuccess(response, err, appearance);
    }
};


DECLARE_CMD(GetMaxWhitelistSizeCommand) {
    CMD_NAME("getMaxWhitelistSize")
    CMD_HELP("get the maximum size the whitelist can take")

    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getMaxWhitelistSize());
    }
};


DECLARE_CMD(GetWhitelistCommand) {
    CMD_NAME("getWhitelist")
    CMD_HELP("Get the internal whitelist to be used by the Link Layer when scanning,"
               "advertising or initiating a connection depending on the filter policies.")

    CMD_RESULTS(
        CMD_RESULT("JSON Array", "", "Array of the address in the whitelist"),
        CMD_RESULT("JSON Object", "[i]", "Description of an address"),
        CMD_RESULT("AddressType_t", "[i].address_type", "Type of the address"),
        CMD_RESULT("MacAddress_t", "[i].address", "The mac address"),
    )

    CMD_HANDLER(CommandResponsePtr& response) {
        using namespace serialization;

        BLEProtocol::Address_t* addresses = new BLEProtocol::Address_t[gap().getMaxWhitelistSize()]();
        Gap::Whitelist_t whiteList = {
            addresses,
            /* size */ 0,
            /* capacity */ gap().getMaxWhitelistSize()
        };

        ble_error_t err = gap().getWhitelist(whiteList);
        if(err) {
            response->faillure(err);
            delete[] addresses;
            return;
        }

        response->success();
        serialization::JSONOutputStream& os = response->getResultStream();

        os << startArray;
        for(std::size_t i = 0; i < whiteList.size; ++i) {
            os << startObject <<
                key("address_type") << whiteList.addresses[i].type <<
                key("address") << whiteList.addresses[i].address <<
            endObject;
        }
        os << endArray;

        delete[] addresses;
    }
};


DECLARE_CMD(SetWhitelistCommand) {
    CMD_NAME("setWhitelist")
    CMD_HELP("Set the internal whitelist to be used by the Link Layer when scanning,"
               "advertising or initiating a connection depending on the filter policies.")

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
        if(args.count() % 2) {
            response->invalidParameters("[ <addressType> <address> ] expected");
            return;
        }

        uint8_t addressCount = args.count() / 2;
        BLEProtocol::Address_t* addresses = new BLEProtocol::Address_t[addressCount]();

        // fill the input
        for(uint8_t i = 0; i < addressCount; ++i) {
            if(!fromString(args[i * 2], addresses[i].type)) {
                response->invalidParameters("invalid address type");
                delete[] addresses;
                return;
            }

            if(!macAddressFromString(args[(i * 2) + 1], addresses[i].address)) {
                response->invalidParameters("invalid address");
                delete[] addresses;
                return;
            }
        }

        Gap::Whitelist_t whiteList = {
            addresses,
            /* size */ addressCount,
            /* capacity */ addressCount
        };
        ble_error_t err = gap().setWhitelist(whiteList);
        delete[] addresses;

        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(EnablePrivacyCommand) {
    CMD_NAME("enablePrivacy")
    CMD_HELP("Enable or disable the privacy")

    CMD_ARGS(
        CMD_ARG("bool", "enable", "Enable or disable the privacy")
    )

    CMD_HANDLER(bool enable, CommandResponsePtr& response) {
        reportErrorOrSuccess(response, gap().enablePrivacy(enable));
    }
};


DECLARE_CMD(SetPeripheralPrivacyConfigurationCommand) {
    CMD_NAME("setPeripheralPrivacyConfiguration")
    CMD_HELP("Set the peripheral privacy configuration.")

    CMD_ARGS(
        CMD_ARG("bool", "use_non_resolvable_random_address", "Use non resolvable address in non connectable advertisements"),
        CMD_ARG("Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t", "resolution_strategy", "Strategy used to resolve addresses present in scan and connection requests.")
    )

    CMD_HANDLER(
        bool use_non_resolvable_random_address,
        Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t& resolution_strategy,
        CommandResponsePtr& response
    ) {
        Gap::PeripheralPrivacyConfiguration_t configuration = {
            use_non_resolvable_random_address,
            resolution_strategy
        };

        reportErrorOrSuccess(
            response,
            gap().setPeripheralPrivacyConfiguration(&configuration)
        );
    }
};


DECLARE_CMD(GetPeripheralPrivacyConfigurationCommand) {
    CMD_NAME("getPeripheralPrivacyConfiguration")
    CMD_HELP("Get the peripheral privacy configuration.")

    CMD_RESULTS(
        CMD_RESULT("bool", "use_non_resolvable_random_address", "Indicates if non resolvable addresses are used in non connectable advertisements."),
        CMD_RESULT("Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t", "resolution_strategy", "Strategy used to resolve address in scan and connection requests."),
    )

    CMD_HANDLER(CommandResponsePtr& response) {
        Gap::PeripheralPrivacyConfiguration_t configuration;

        reportErrorOrSuccess(
            response,
            gap().getPeripheralPrivacyConfiguration(&configuration),
            configuration
        );
    }
};



DECLARE_CMD(SetCentralPrivacyConfigurationCommand) {
    CMD_NAME("setCentralPrivacyConfiguration")
    CMD_HELP("Set the central privacy configuration.")

    CMD_ARGS(
        CMD_ARG("bool", "use_non_resolvable_random_address", "Use non resolvable address in scan requests."),
        CMD_ARG("Gap::CentralPrivacyConfiguration_t::resolution_strategy_t", "resolution_strategy", "Strategy used to resolve addresses present in advertisement packets.")
    )

    CMD_HANDLER(
        bool use_non_resolvable_random_address,
        Gap::CentralPrivacyConfiguration_t::resolution_strategy_t& resolution_strategy,
        CommandResponsePtr& response
    ) {
        Gap::CentralPrivacyConfiguration_t configuration = {
            use_non_resolvable_random_address,
            resolution_strategy
        };

        reportErrorOrSuccess(
            response,
            gap().setCentralPrivacyConfiguration(&configuration)
        );
    }
};


DECLARE_CMD(GetCentralPrivacyConfigurationCommand) {
    CMD_NAME("getCentralPrivacyConfiguration")
    CMD_HELP("Get the central privacy configuration.")

    CMD_RESULTS(
        CMD_RESULT("bool", "use_non_resolvable_random_address", "Indicates if non resolvable addresses are used in scan request."),
        CMD_RESULT("Gap::CentralPrivacyConfiguration_t::resolution_strategy_t", "resolution_strategy", "Strategy used to resolve addresses in advertisements."),
    )

    CMD_HANDLER(CommandResponsePtr& response) {
        Gap::CentralPrivacyConfiguration_t configuration;

        reportErrorOrSuccess(
            response,
            gap().getCentralPrivacyConfiguration(&configuration),
            configuration
        );
    }
};


DECLARE_CMD(ReadPhyCommand) {
    CMD_NAME("readPhy")
    CMD_HELP("Read current PHY of the connection.")

    CMD_ARGS(
        CMD_RESULT("uint16_t", "handle", "The handle of the connection queried")
    )

    CMD_HANDLER(
        Gap::Handle_t handle,
        CommandResponsePtr& response
    ) {
        startProcedure<ReadPhyProcedure>(handle, response, 1000 /* timeout in ms */);
    }

    struct ReadPhyProcedure : public AsyncProcedure, Gap::EventHandler {
        ReadPhyProcedure(
            Gap::Handle_t connectionHandle,
            const SharedPointer<CommandResponse>& response,
            uint32_t procedureTimeout
        ) : AsyncProcedure(response, procedureTimeout), handle(connectionHandle) { }

        virtual ~ReadPhyProcedure() {
            gap().setEventHandler(NULL);
        }

        virtual bool doStart() {
            gap().setEventHandler(this);
            ble_error_t result = gap().readPhy(handle);
            if (result != BLE_ERROR_NONE) {
                reportErrorOrSuccess(
                    response,
                    result
                );
                return false;
            }
            return true;
        }

        void onReadPhy(
            ble_error_t status,
            ble::connection_handle_t connectionHandle,
            ble::phy_t txPhy,
            ble::phy_t rxPhy
        ) {
            serialization::JSONOutputStream& os = response->getResultStream();

            response->success();

            os << serialization::startObject <<
                serialization::key("handle") << connectionHandle <<
                serialization::key("txPhy") << txPhy <<
                serialization::key("rxPhy") << rxPhy <<
            serialization::endObject;

            terminate();
        }

        Gap::Handle_t handle;
    };
};


DECLARE_CMD(SetPhyCommand) {
    CMD_NAME("setPhy")
    CMD_HELP("Set PHY preference for given connection.")

    CMD_ARGS(
        CMD_RESULT("uint16_t", "handle", "The handle of the connection queried"),
        CMD_RESULT("uint8_t", "tx_phy", "Preferred tx PHYs mask"),
        CMD_RESULT("uint8_t", "rx_phy", "Preferred rx PHYs mask"),
        CMD_RESULT("uint8_t", "coded_symbol", "Preferred types of coding")
    )

    CMD_HANDLER(
        Gap::Handle_t handle,
        uint8_t tx_phy,
        uint8_t rx_phy,
        uint8_t coded_symbol,
        CommandResponsePtr& response
    ) {
        reportErrorOrSuccess(
            response,
            gap().setPhy(
                handle,
                (ble::phy_set_t*)&tx_phy,
                (ble::phy_set_t*)&rx_phy,
                (ble::coded_symbol_per_bit_t::type)coded_symbol
            )
        );
    }
};


DECLARE_CMD(SetPreferedPhysCommand) {
    CMD_NAME("setPreferedPhys")
    CMD_HELP("Set PHY preference for all connections.")

    CMD_ARGS(
        CMD_RESULT("uint8_t", "tx phy", "Preferred tx PHYs mask"),
        CMD_RESULT("uint8_t", "rx phy", "Preferred rx PHYs mask")
    )

    CMD_HANDLER(
        uint8_t tx_phy,
        uint8_t rx_phy,
        CommandResponsePtr& response
    ) {
        reportErrorOrSuccess(
            response,
            gap().setPreferredPhys(
                (ble::phy_set_t*)&tx_phy,
                (ble::phy_set_t*)&rx_phy
            )
        );
    }
};

bool use_version(uint8_t);

DECLARE_CMD(UseVersion) {
    CMD_NAME("useVersion")
    CMD_HELP("Use a specific version of the API.")
    CMD_HANDLER(uint8_t version, CommandResponsePtr& response) {
        bool success = use_version(version);
        if (success) {
            response->success();
        } else {
            response->faillure();
        }
    }
};

static const Command* const _commonCommandHandlers[] = {
    CMD_INSTANCE(SetAddressCommand),
    CMD_INSTANCE(GetAddressCommand),
    CMD_INSTANCE(GetMinAdvertisingIntervalCommand),
    CMD_INSTANCE(GetMinNonConnectableAdvertisingIntervalCommand),
    CMD_INSTANCE(GetMaxAdvertisingIntervalCommand),
    CMD_INSTANCE(SetDeviceNameCommand),
    CMD_INSTANCE(GetDeviceNameCommand),
    CMD_INSTANCE(SetAppearanceCommand),
    CMD_INSTANCE(GetAppearanceCommand),
    CMD_INSTANCE(GetMaxWhitelistSizeCommand),
    CMD_INSTANCE(GetWhitelistCommand),
    CMD_INSTANCE(SetWhitelistCommand),
    CMD_INSTANCE(EnablePrivacyCommand),
    CMD_INSTANCE(SetPeripheralPrivacyConfigurationCommand),
    CMD_INSTANCE(GetPeripheralPrivacyConfigurationCommand),
    CMD_INSTANCE(SetCentralPrivacyConfigurationCommand),
    CMD_INSTANCE(SetPhyCommand),
    CMD_INSTANCE(SetPreferedPhysCommand),
    CMD_INSTANCE(ReadPhyCommand),
    CMD_INSTANCE(UseVersion)
};

static const Command** _commands = NULL;
static size_t _commands_count = 0;

bool use_version(uint8_t version) {
    static uint8_t _version = 0;

    if (version != 1 && version != 2) {
        return false;
    }

    if (_version == version) {
        return true;
    }

    delete[] _commands;

    const size_t common_commands_count =
        sizeof(_commonCommandHandlers) / sizeof(_commonCommandHandlers[0]);
    ConstArray<const Command*> version_commands = version == 1 ?
        GapV1CommandSuiteDescription::commands() :
        GapV2CommandSuiteDescription::commands();

    _commands_count = common_commands_count + version_commands.count();
    _commands = new const Command*[_commands_count];

    for (size_t i = 0; i < common_commands_count; ++i) {
        _commands[i] = _commonCommandHandlers[i];
    }

    for (size_t i = 0; i < version_commands.count(); ++i) {
        _commands[i + common_commands_count] = version_commands[i];
    }

    if (version == 2) {
        GapV2CommandSuiteDescription::init();
    }

    return true;
}

} // end of annonymous namespace


ConstArray<const Command*> GapCommandSuiteDescription::commands() {
    if (_commands == NULL) {
        use_version(1);
    }

    return ConstArray<const Command*>(_commands_count, _commands);
}
