#include "GapCommands.h"

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/GapAdvertisingDataSerializer.h"
#include "Serialization/GapAdvertisingParamsSerializer.h"
#include "Serialization/BLECommonSerializer.h"
#include "CLICommand/CommandSuite.h"
#include "util/AsyncProcedure.h"

#ifdef YOTTA_CFG
#include "mbed-drivers/Timer.h"
#else
#include "Timer.h"
#endif

using mbed::util::SharedPointer;

// isolation ...
namespace {

/**
 * return the ble instance of this device
 */
static BLE& ble() {
    return BLE::Instance();
}

/**
 * return the Gap instance of this device
 */
static Gap& gap() {
    return ble().gap();
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


struct SetAddressCommand : public BaseCommand {
    static const char* name() {
        return "setAddress";
    }

    static const char* help() {
        return "set the address of this device";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<addressType>", "The address type to set. It is a string representation of BLEProtocol::AddressType_t" },
            { "<address>"    , "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\"" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        // extract first args
        BLEProtocol::AddressType_t addressType;
        if(!fromString(args[0], addressType)) {
            response->invalidParameters("first argument should match BLEProtocol::AddressType_t");
            return;
        }

        Gap::Address_t address;
        if(!macAddressFromString(args[1], address)) {
            response->invalidParameters("second argument should is a mac address which should match XX:XX:XX:XX:XX:XX format");
            return;
        }

        ble_error_t err = gap().setAddress(addressType, address);
        reportErrorOrSuccess(response, err);
    }
};


struct GetAddressCommand : public BaseCommand {
    static const char* name() {
        return "getAddress";
    }

    static const char* help() {
        return "Get the address and the type of address of this device.\r\n"
               "The result will be a json object containing:\r\n"
               "   * 'address_type': <type of the address. It is a string representation of BLEProtocol::AddressType_t>\r\n"
               "   * 'address'     : <the address which is a string representation like 'XX:XX:XX:XX:XX:XX'>\r\n";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
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


struct GetMinAdvertisingIntervalCommand : public BaseCommand {
    static const char* name() {
        return "getMinAdvertisingInterval";
    }

    static const char* help() {
        return "Return the minimum advertising interval";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getMinNonConnectableAdvertisingInterval());
    }
};


struct GetMinNonConnectableAdvertisingIntervalCommand : public BaseCommand {
    static const char* name() {
        return "getMinNonConnectableAdvertisingInterval";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getMinNonConnectableAdvertisingInterval());
    }
};


struct GetMaxAdvertisingIntervalCommand : public BaseCommand {
    static const char* name() {
        return "getMaxAdvertisingInterval";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getMaxAdvertisingInterval());
    }
};


struct StopAdvertisingCommand : public BaseCommand {
    static const char* name() {
        return "stopAdvertising";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().stopAdvertising();
        reportErrorOrSuccess(response, err);
    }
};


struct StopScanCommand : public BaseCommand {
    static const char* name() {
        return "stopScan";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().stopScan();
        reportErrorOrSuccess(response, err);
    }
};


struct ConnectCommand : public BaseCommand {
    static const char* name() {
        return "connect";
    }

    static const char* help() {
        return  "connect to a device, if this function succeed, a ConnectionCallbackParams_t is returned:\r\n"\
                "\thandle: The connection handle\r\n"\
                "\trole: Role of the device in the connection (here, it should be central)\r\n"\
                "\tpeerAddrType: The addressType of the peer\r\n"\
                "\tpeerAddr: The address of the peer\r\n"\
                "\townAddrType: The address type of this device\r\n"\
                "\townAddr: The address of this device\r\n"\
                "\tconnectionParams: Object which contain the parameters of the connection\r\n"\
                "\t\tminConnectionInterval: minimum connection interval for this connection\r\n"\
                "\t\tmaxConnectionInterval: maximum connection interval for this connection\r\n"\
                "\t\tslaveLatency: slave latency of the connection\r\n"\
                "\t\tconnectionSupervisionTimeout: supervision timeout for this connection";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<addressType>", "The address type to of the peer device."\
                               "It is a string representation of BLEProtocol::AddressType_t" },
            { "<address>", "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\"" },
            // connection parameters
            { "<minConnectionInterval>", "Minimum Connection Interval in 1.25 ms units" },
            { "<maxConnectionInterval>", "Maximum Connection Interval in 1.25 ms units" },
            { "<slaveLatency>", "Slave Latency in number of connection events" },
            { "<connectionSupervisionTimeout>", "Connection Supervision Timeout in 10 ms units" },
            // scan parameters
            { "<interval>", "The scan interval, it should be a number between 3 and 10420ms." },
            { "<window>", "The scan window, it should be a number between 3 and 10420ms." },
            { "<scan timeout>", "The scan timeout, it should be a number between 0 and 65534" },
            { "<activeScanning>", "A boolean value { true, false } indicating if the device send scan request or not" },
            // timeout for this procedure
            { "<timeout>", "Maximum time allowed for this procedure" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        // extract Address and address type
        BLEProtocol::AddressType_t addressType;
        if (!fromString(args[0], addressType)) {
            response->invalidParameters("first argument should match BLEProtocol::AddressType_t");
            return;
        }

        Gap::Address_t address;
        if (!macAddressFromString(args[1], address)) {
            response->invalidParameters(
                "second argument should is a mac address which should match XX:XX:XX:XX:XX:XX format"
            );
            return;
        }

        // extract connection parameters
        uint16_t minConnectionInterval;
        if (!fromString(args[2], minConnectionInterval)) {
            response->invalidParameters("the minimum connection interval is ill formed");
            return;
        }

        uint16_t maxConnectionInterval;
        if (!fromString(args[3], maxConnectionInterval)) {
            response->invalidParameters("the maximum connection interval is ill formed");
            return;
        }

        uint16_t slaveLatency;
        if (!fromString(args[4], slaveLatency)) {
            response->invalidParameters("the slave latency is ill formed");
            return;
        }

        uint16_t connectionSupervisionTimeout;
        if (!fromString(args[5], connectionSupervisionTimeout)) {
            response->invalidParameters("the connection supervision timeout is ill formed");
            return;
        }

        // extract scan parameters
        uint16_t scanInterval;
        if (!fromString(args[6], scanInterval)) {
            response->invalidParameters("the scan interval is ill formed");
            return;
        }

        uint16_t window;
        if (!fromString(args[7], window)) {
            response->invalidParameters("the scan window is ill formed");
            return;
        }

        uint16_t scanTimeout;
        if (!fromString(args[8], scanTimeout)) {
            response->invalidParameters("the scan timeout is ill formed");
            return;
        }

        bool activeScanning;
        if (!fromString(args[9], activeScanning)) {
            response->invalidParameters("the active scanning is ill formed");
            return;
        }

        // timeout for this procedure
        uint16_t procedureTimeout;
        if (!fromString(args[10], procedureTimeout)) {
            response->invalidParameters("the procedure timeout is ill formed");
            return;
        }

        // everything is alright, launching the procedure
        Gap::ConnectionParams_t connectionParams = {
            minConnectionInterval,
            maxConnectionInterval,
            slaveLatency,
            connectionSupervisionTimeout
        };

        GapScanningParams scanParams(
            scanInterval,
            window,
            scanTimeout,
            activeScanning
        );

        ble_error_t err = gap().connect(
            address,
            addressType,
            &connectionParams,
            &scanParams
        );

        if (err) {
            response->faillure(err);
            return;
        }

        startProcedure<ConnectionProcedure>(addressType, address, response, procedureTimeout);
    }

    struct ConnectionProcedure : public AsyncProcedure {
        ConnectionProcedure(BLEProtocol::AddressType_t _addressType, const Gap::Address_t& _address,
            const SharedPointer<CommandResponse>& res, uint32_t procedureTimeout) :
            AsyncProcedure(res, procedureTimeout), addressType(_addressType) {
            memcpy(address, _address, sizeof(address));
            gap().onConnection(makeFunctionPointer(this, &ConnectionProcedure::whenConnected));
        }

        virtual ~ConnectionProcedure() {
            gap().onConnection().detach(makeFunctionPointer(this, &ConnectionProcedure::whenConnected));
        }

        virtual bool doStart() {
            // nothing to do here, already started ...
            return true;
        }

        void whenConnected(const Gap::ConnectionCallbackParams_t* params) {
            using namespace serialization;

            // check that the callback has been called for the right address and object
            if (memcmp(params->peerAddr, address, sizeof(address)) != 0 ||
                params->peerAddrType != addressType) {
                return;
            }

            response->success();
            serialization::JSONOutputStream& os = response->getResultStream();

            os << startObject <<
                key("handle") << params->handle <<
                key("role") << toString(params->role) <<
                key("peerAddrType") << toString(params->peerAddrType) <<
                key("peerAddr") << macAddressToString(params->peerAddr).str <<
                key("ownAddrType") << toString(params->ownAddrType) <<
                key("ownAddr") << macAddressToString(params->ownAddr).str <<
                key("connectionParams") << startObject <<
                    key("minConnectionInterval") << params->connectionParams->minConnectionInterval <<
                    key("maxConnectionInterval") << params->connectionParams->maxConnectionInterval <<
                    key("slaveLatency") << params->connectionParams->slaveLatency <<
                    key("connectionSupervisionTimeout") << params->connectionParams->connectionSupervisionTimeout <<
                endObject <<
            endObject;

            terminate();
        }

        BLEProtocol::AddressType_t addressType;
        Gap::Address_t address;
    };
};


struct WaitForConnectionCommand : public BaseCommand {
    static const char* name() {
        return "waitForConnection";
    }

    static const char* help() {
        return "Wait for a connection to occur";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<timeout>", "Maximum time allowed for this procedure" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        // timeout for this procedure
        uint16_t procedureTimeout;
        if (!fromString(args[0], procedureTimeout)) {
            response->invalidParameters("the procedure timeout is ill formed");
            return;
        }

        startProcedure<WaitForConnectionProcedure>(response, procedureTimeout);
    }

    struct WaitForConnectionProcedure : public AsyncProcedure {
        WaitForConnectionProcedure(const SharedPointer<CommandResponse>& res, uint32_t procedureTimeout) :
            AsyncProcedure(res, procedureTimeout) {
            gap().onConnection(makeFunctionPointer(this, &WaitForConnectionProcedure::whenConnected));
        }

        virtual ~WaitForConnectionProcedure() {
            gap().onConnection().detach(makeFunctionPointer(this, &WaitForConnectionProcedure::whenConnected));
        }

        virtual bool doStart() {
            // nothing to do here, already started ...
            return true;
        }

        void whenConnected(const Gap::ConnectionCallbackParams_t* params) {
            using namespace serialization;

            response->success();
            serialization::JSONOutputStream& os = response->getResultStream();

            os << startObject <<
                key("handle") << params->handle <<
                key("role") << toString(params->role) <<
                key("peerAddrType") << toString(params->peerAddrType) <<
                key("peerAddr") << macAddressToString(params->peerAddr).str <<
                key("ownAddrType") << toString(params->ownAddrType) <<
                key("ownAddr") << macAddressToString(params->ownAddr).str <<
                key("connectionParams") << startObject <<
                    key("minConnectionInterval") << params->connectionParams->minConnectionInterval <<
                    key("maxConnectionInterval") << params->connectionParams->maxConnectionInterval <<
                    key("slaveLatency") << params->connectionParams->slaveLatency <<
                    key("connectionSupervisionTimeout") << params->connectionParams->connectionSupervisionTimeout <<
                endObject <<
            endObject;

            terminate();
        }
    };
};


struct DisconnectCommand : public BaseCommand {
    static const char* name() {
        return "disconnect";
    }

    static const char* help() {
        return "disconnect the device from a specific connection.\r\n"\
               "If procedure succeed, a JSON object containing the following fields will be returned:\r\n"\
               "\t* handle: The handle disconnected\r\n"\
               "\t* reason: The reason of the disconnection (see Gap::DisconnectionReason_t)\r\n"\
               "In case of error, the reason of the error will be returned.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The id of the connection to terminate." },
            { "<reason>", "The reason of the termination (see Gap::DisconnectionReason_t)" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        static Gap::Handle_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        Gap::DisconnectionReason_t reason;
        if (!fromString(args[1], reason)) {
            response->invalidParameters("the disconnection reason is ill formed");
            return;
        }

        // Set the timeout to the maximum supervision timeout allowed in BLE
        startProcedure<DisconnectionProcedure>(connectionHandle, reason, response, 10240);
    }

    struct DisconnectionProcedure : public AsyncProcedure {
        DisconnectionProcedure(Gap::Handle_t handle, Gap::DisconnectionReason_t disconnectionReason,
            const SharedPointer<CommandResponse>& res, uint16_t procedureTimeout) :
            AsyncProcedure(res, procedureTimeout), connectionHandle(handle), reason(disconnectionReason) {
            gap().onDisconnection(this, &DisconnectionProcedure::whenDisconnected);
        }

        virtual ~DisconnectionProcedure() {
            gap().onDisconnection().detach(makeFunctionPointer(this, &DisconnectionProcedure::whenDisconnected));
        }

        virtual bool doStart() {
            ble_error_t err = gap().disconnect(connectionHandle, reason);
            if (err) {
                response->faillure(err);
                return false;
            }
            return true;
        }

        void whenDisconnected(const Gap::DisconnectionCallbackParams_t* params) {
            using namespace serialization;

            if (params->handle != connectionHandle) {
                return;
            }

            // the right connection has been disconnected, sending result back to the user
            response->success();
            response->getResultStream() << startObject <<
                key("handle") << params->handle <<
                key("reason") << params->reason <<
            endObject;

            terminate();
        }

        Gap::Handle_t connectionHandle;
        Gap::DisconnectionReason_t reason;
    };
};


struct GetPreferredConnectionParamsCommand : public BaseCommand {
    static const char* name() {
        return "getPreferredConnectionParams";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        Gap::ConnectionParams_t connectionParameters;
        ble_error_t err = gap().getPreferredConnectionParams(&connectionParameters);
        reportErrorOrSuccess(response, err, connectionParameters);
    }
};


struct SetPreferredConnectionParamsCommand : public BaseCommand {
    static const char* name() {
        return "setPreferredConnectionParams";
    }

    static const char* help() {
        return "set the prefered connection parameters";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>", "all the parameters, coma separated" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        Gap::ConnectionParams_t connectionParameters;
        if(!connectionParamsFromCLI(args[0], connectionParameters)) {
            response->invalidParameters("malformed connection parameters, should be like"\
                "<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>");
            return;
        }

        ble_error_t err =  gap().setPreferredConnectionParams(&connectionParameters);
        reportErrorOrSuccess(response, err);
    }
};


struct UpdateConnectionParamsCommand : public BaseCommand {
    static const char* name() {
        return "updateConnectionParams";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct SetDeviceNameCommand : public BaseCommand {
    static const char* name() {
        return "setDeviceName";
    }

    static const char* help() {
        return "set the devce name";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<name>", "the name of the device, it should not have space" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().setDeviceName((const uint8_t*) args[0]);
        reportErrorOrSuccess(response, err);
    }
};


struct GetDeviceNameCommand : public BaseCommand {
    static const char* name() {
        return "getDeviceName";
    }

    static const char* help() {
        return "return the device name as a string";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
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


struct SetAppearanceCommand : public BaseCommand {
    static const char* name() {
        return "setAppearance";
    }

    static const char* help() {
        return "set the appearance flag of the device";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<appearance>", "the appearance to set (see GapAdvertisingData::Appearance_t)" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        if(!fromString(args[0], appearance)) {
            response->invalidParameters("the appearance to set is illformed (see GapAdvertisingData::Appearance_t)");
            return;
        }

        ble_error_t err = gap().setAppearance(appearance);
        reportErrorOrSuccess(response, err);
    }
};


struct GetAppearanceCommand : public BaseCommand {
    static const char* name() {
        return "getAppearance";
    }

    static const char* help() {
        return "get the appearance of the device";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        ble_error_t err = gap().getAppearance(&appearance);
        reportErrorOrSuccess(response, err, appearance);
    }
};


struct SetTxPowerCommand : public BaseCommand {
    static const char* name() {
        return "setTxPower";
    }

    static const char* help() {
        return "set the transmission power of the device";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<TxPower>", "The transmission power, it is an integer between [-128:127]"}
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        int8_t txPower = 0;
        if(!fromString(args[0], txPower)) {
            response->invalidParameters("the txPower is malformed (should be between [-127:128])");
            return;
        }

        ble_error_t err = gap().setTxPower(txPower);
        reportErrorOrSuccess(response, err);
    }
};


struct GetPermittedTxPowerValuesCommand : public BaseCommand {
    static const char* name() {
        return "getPermittedTxPowerValues";
    }

    static const char* help() {
        return "return an array of the authorized Tx power values";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        using namespace serialization;
        const int8_t* permittedTxPowerValues = NULL;
        size_t permittedTxPowerValuesCount = 0;

        gap().getPermittedTxPowerValues(&permittedTxPowerValues, &permittedTxPowerValuesCount);

        response->success();
        serialization::JSONOutputStream& os = response->getResultStream();
        os << startArray;
        for (std::size_t i = 0; i < permittedTxPowerValuesCount; ++i) {
            os << permittedTxPowerValues[i];
        }
        os << endArray;
    }
};


struct GetStateCommand : public BaseCommand {
    static const char* name() {
        return "getState";
    }

    static const char* help() {
        return "return the state of the device as defined in Gap::GapState_t";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getState());
    }
};


struct SetAdvertisingTypeCommand : public BaseCommand {
    static const char* name() {
        return "setAdvertisingType";
    }

    static const char* help() {
        return "set the advertising type";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<advertisingType>", "The advertising type as defined in GapAdvertisingParams::AdvertisingType_t" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        GapAdvertisingParams::AdvertisingType_t advType;

        if(!fromString(args[0], advType)) {
            response->invalidParameters("the advertising type is incorrect (see GapAdvertisingParams::AdvertisingType_t)");
            return;
        }

        gap().setAdvertisingType(advType);
        response->success();
    }
};


struct SetAdvertisingIntervalCommand : public BaseCommand {
    static const char* name() {
        return "setAdvertisingInterval";
    }

    static const char* help() {
        return "set the advertising interval";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<interval>", "The interval in ms, if 0, the advertising is disabled" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            response->invalidParameters("the advertising interval is ill formed");
            return;
        }

        gap().setAdvertisingInterval(interval);
        response->success();
    }
};


struct SetAdvertisingTimeoutCommand : public BaseCommand {
    static const char* name() {
        return "setAdvertisingTimeout";
    }

    static const char* help() {
        return "set the advertising timeout, in seconds";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<timeout>", "An integer wich represent the advertising timeout in seconds [0x1 : 0x3FFF]. 0 disable the timeout" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            response->invalidParameters("the advertising timeout is ill formed");
            return;
        }

        gap().setAdvertisingTimeout(timeout);
        response->success();
    }
};


struct StartAdvertisingCommand : public BaseCommand {
    static const char* name() {
        return "startAdvertising";
    }

    static const char* help() {
        return "start the advertising";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().startAdvertising();
        reportErrorOrSuccess(response, err);
    }
};


struct ClearAdvertisingPayloadCommand : public BaseCommand {
    static const char* name() {
        return "clearAdvertisingPayload";
    }

    static const char* help() {
        return "clear the advertising payload";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        gap().clearAdvertisingPayload();
        response->success();
    }
};


struct AccumulateAdvertisingPayloadCommand : public BaseCommand {
    static const char* name() {
        return "accumulateAdvertisingPayload";
    }

    static const char* help() {
        return "add a new field into the advertising payload";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<fieldType>", "the field type of the data following (see GapAdvertisingData::DataType_t)" },
            { "<data>", "the value of the field, please see GapAdvertisingData" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        AdvertisingPayloadField_t payloadField;
        const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

        if(parsingError) {
            response->invalidParameters(parsingError);
            return;
        }

        ble_error_t err = gap().accumulateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
        reportErrorOrSuccess(response, err);
    }
};


struct UpdateAdvertisingPayloadCommand : public BaseCommand {
    static const char* name() {
        return "updateAdvertisingPayload";
    }

    static const char* help() {
        return "update a field int the advertising payload.\r\n"
               "Take care, at the moment, this will only succeed if the new value has the same size as the old one";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
            { "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        AdvertisingPayloadField_t payloadField;
        const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

        if(parsingError) {
            response->invalidParameters(parsingError);
            return;
        }

        ble_error_t err = gap().updateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
        reportErrorOrSuccess(response, err);
    }
};


struct SetAdvertisingPayloadCommand : public BaseCommand {
    static const char* name() {
        return "setAdvertisingPayload";
    }

    static const char* help() {
        return "set the advertising payload";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        // TODO
        //ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
        response->notImplemented();
    }
};


struct GetAdvertisingPayloadCommand : public BaseCommand {
    static const char* name() {
        return "getAdvertisingPayload";
    }

    static const char* help() {
        return "get the current advertising payload";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getAdvertisingPayload());
    }
};


struct AccumulateScanResponseCommand : public BaseCommand {
    static const char* name() {
        return "accumulateScanResponse";
    }

    static const char* help() {
        return "add a field into the scan response payload";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
            { "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        AdvertisingPayloadField_t payloadField;
        const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

        if(parsingError) {
            response->invalidParameters(parsingError);
            return;
        }

        ble_error_t err = gap().accumulateScanResponse(payloadField.dataType, payloadField.data, payloadField.dataLenght);
        reportErrorOrSuccess(response, err);
    }
};


struct ClearScanResponseCommand : public BaseCommand {
    static const char* name() {
        return "clearScanResponse";
    }

    static const char* help() {
        return "clear the scan response";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        gap().clearScanResponse();
        response->success();
    }
};


struct SetScanParamsCommand : public BaseCommand {
    static const char* name() {
        return "setScanParams";
    }

    static const char* help() {
        return "Set the scan parameters";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<interval>", "The scan interval, it should be a number between 3 and 10420ms" },
            { "<window>", "The scan window, it should be a number between 3 and 10420ms" },
            { "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " },
            { "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t interval = 0xFFFF;
        if(!fromString(args[0], interval)) {
            response->invalidParameters("invalid interval, it should be a number between 3 and 10240ms");
            return;
        }

        uint16_t window = 0xFFFF;
        if(!fromString(args[1], window)) {
            response->invalidParameters("invalid window, it should be a number between 3 and 10240ms");
            return;
        }

        uint16_t timeout = 0;
        if(!fromString(args[2], timeout)) {
            response->invalidParameters("invalid timeout, it should be a number between 0 and 65534");
            return;
        }

        uint16_t activeScanning = 0;
        if(!fromString(args[3], activeScanning)) {
            response->invalidParameters("invalid activeScaning, it should be a number boolean value");
            return;
        }

        ble_error_t err = gap().setScanParams(interval, window, timeout, activeScanning);
        reportErrorOrSuccess(response, err);
    }
};


struct SetScanIntervalCommand : public BaseCommand {
    static const char* name() {
        return "setScanInterval";
    }

    static const char* help() {
        return "set the scan interval";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<interval>", "The interval between each scan, it should be a number between 3 and 10240ms" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            response->invalidParameters("the interval is ill formed");
            return;
        }

        ble_error_t err = gap().setScanInterval(interval);
        reportErrorOrSuccess(response, err);
    }
};


struct SetScanWindowCommand : public BaseCommand {
    static const char* name() {
        return "setScanWindow";
    }

    static const char* help() {
        return "set the scan windows";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<window>", "The interval between each scan, it should be a number between 3 and 10240ms" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t window = 0;
        if(!fromString(args[0], window)) {
            response->invalidParameters("the window is ill formed");
            return;
        }

        ble_error_t err = gap().setScanWindow(window);
        reportErrorOrSuccess(response, err);
    }
};


struct SetScanTimeoutCommand : public BaseCommand {
    static const char* name() {
        return "setScanTimeout";
    }

    static const char* help() {
        return "Set the scane timeout";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            response->invalidParameters("the timeout is ill formed");
            return;
        }

        ble_error_t err = gap().setScanTimeout(timeout);
        reportErrorOrSuccess(response, err);
    }
};


struct SetActiveScanningCommand : public BaseCommand {
    static const char* name() {
        return "setActiveScanning";
    }

    static const char* help() {
        return "Enable or disable active scanning";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        bool activeScanning = 0;
        if(!fromString(args[0], activeScanning)) {
            response->invalidParameters("the active scanning state is ill formed");
            return;
        }

        ble_error_t err = gap().setActiveScanning(activeScanning);
        reportErrorOrSuccess(response, err);
    }
};


struct StartScanCommand : public BaseCommand {
    static const char* name() {
        return "startScan";
    }

    static const char* help() {
        return "start the scan process";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<duration>", "The duration of the scan" },
            { "<address>", "The address to scan for" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t duration = 0;
        if(!fromString(args[0], duration)) {
            response->invalidParameters("first argument should be the duration of the scan in milliseconds");
            return;
        }

        Gap::Address_t address;
        if(!macAddressFromString(args[1], address)) {
            response->invalidParameters("second argument should be a mac address which should match XX:XX:XX:XX:XX:XX format");
            return;
        }

        startProcedure<ScanProcedure>(response, duration, address);
    }

    struct ScanProcedure : public AsyncProcedure {
        ScanProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, const Gap::Address_t& addr) :
            AsyncProcedure(res, timeout) {
                memcpy(address, addr, sizeof(address));
        }

        virtual ~ScanProcedure() {
            // close the array of scan
            gap().stopScan();
            timer.stop();
            // note : there should be a way to detach this function pointer in the BLE API
        }

        virtual bool doStart() {
            ble_error_t err = gap().startScan(this, &ScanProcedure::whenAdvertisementReceived);
            if(err) {
                response->faillure(err);
                return false;
            } else {
                timer.reset();
                timer.start();
                response->success();
                // the response will be an array of scan sample, start this array right now
                response->getResultStream() << serialization::startArray;
                return true;
            }
        }

        void whenAdvertisementReceived(const Gap::AdvertisementCallbackParams_t* scanResult) {
            using namespace serialization;

            // check the address, if it is not the address wanted, just return
            if(memcmp(scanResult->peerAddr, address, sizeof(address))) {
                return;
            }

            response->getResultStream() << startObject <<
                key("peerAddr") << macAddressToString(scanResult->peerAddr).str <<
                key("rssi") << scanResult->rssi <<
                key("isScanResponse") << scanResult->isScanResponse <<
                key("type") << scanResult->type <<
                key("data") << AdvertisingDataSerializer(scanResult->advertisingData, scanResult->advertisingDataLen) <<
                key("time") << (int32_t) timer.read_ms() <<
            endObject;
        }

        virtual void doWhenTimeout() {
            response->getResultStream() << serialization::endArray;
            // nothing to do jere, timeout is not an error in this case
        }

        Gap::Address_t address;
        mbed::Timer timer;
    };
};


struct InitRadioNotificationCommand : public BaseCommand {
    static const char* name() {
        return "initRadioNotification";
    }

    static const char* help() {
        return "initialize radio notifications";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        // TODO (maybe)
        //ble_error_t initRadioNotification(void)
        response->notImplemented();
    }
};


struct GetAdvertisingParamsCommand : public BaseCommand {
    static const char* name() {
        return "getAdvertisingParams";
    }

    static const char* help() {
        return "return the current advertising params";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getAdvertisingParams());
    }
};


struct SetAdvertisingParamsCommand : public BaseCommand {
    static const char* name() {
        return "setAdvertisingParams";
    }

    static const char* help() {
        return "set the advertising parameters";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<advertisingType>", "The Advertising type, please refer to GapAdvertisingParams::AdvertisingType_t" },
            { "<interval>", "The advertising interval, it should be a number between 0 and 65534" },
            { "<timeout>", "The timeout, it should be a number between 0 and 65534" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        GapAdvertisingParams::AdvertisingType_t advertisingType;
        if(!fromString(args[0], advertisingType)) {
            response->invalidParameters("Advertising type is malformed, please refer to GapAdvertisingParams::AdvertisingType_t");
            return;
        }

        uint16_t interval;
        if(!fromString(args[1], interval)) {
            response->invalidParameters("Advertising interval is malformed, should be a number between 0 and 65534");
            return;
        }

        uint16_t timeout;
        if(!fromString(args[2], timeout)) {
            response->invalidParameters("Advertising timeout is malformed, should be a number between 0 and 65534");
            return;
        }

        gap().setAdvertisingParams(GapAdvertisingParams(advertisingType, interval, timeout));
        response->success();
    }
};


struct GetMaxWhitelistSizeCommand : public BaseCommand {
    static const char* name() {
        return "getMaxWhitelistSize";
    }

    static const char* help() {
        return "get the maximum size the whitelist can take";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getMaxWhitelistSize());
    }
};


struct GetWhitelistCommand : public BaseCommand {
    static const char* name() {
        return "getWhitelist";
    }

    static const char* help() {
        return "Get the internal whitelist to be used by the Link Layer when scanning,"
               "advertising or initiating a connection depending on the filter policies.";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
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


struct SetWhitelistCommand : public BaseCommand {
    static const char* name() {
        return "setWhitelist";
    }

    static const char* help() {
        return "Set the internal whitelist to be used by the Link Layer when scanning,"
               "advertising or initiating a connection depending on the filter policies.";
    }

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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


struct SetAdvertisingPolicyModeCommand : public BaseCommand {
    static const char* name() {
        return "setAdvertisingPolicyMode";
    }

    static const char* help() {
        return "Set the advertising policy filter mode to be used in the next call"
               "to startAdvertising().";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<AdvertisingPolicyMode_t>", "The advertising policy mode to set" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        Gap::AdvertisingPolicyMode_t mode;
        if(!fromString(args[0], mode)) {
            response->invalidParameters("The policy mode in input is ill formed");
            return;
        }

        reportErrorOrSuccess(response, gap().setAdvertisingPolicyMode(mode));
    }
};


struct SetScanningPolicyModeCommand : public BaseCommand {
    static const char* name() {
        return "setScanningPolicyMode";
    }

    static const char* help() {
        return "Set the scan policy filter mode to be used in the next call to startScan().";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<ScanningPolicyMode_t>", "The scanning policy mode to set" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        Gap::ScanningPolicyMode_t mode;
        if(!fromString(args[0], mode)) {
            response->invalidParameters("The policy mode in input is ill formed");
            return;
        }

        reportErrorOrSuccess(response, gap().setScanningPolicyMode(mode));
    }
};


struct SetInitiatorPolicyModeCommand : public BaseCommand {
    static const char* name() {
        return "setInitiatorPolicyMode";
    }

    static const char* help() {
        return "Set the initiator policy filter mode to be used.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<InitiatorPolicyMode_t>", "The scanning policy mode to set" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        Gap::InitiatorPolicyMode_t mode;
        if(!fromString(args[0], mode)) {
            response->invalidParameters("The policy mode in input is ill formed");
            return;
        }

        reportErrorOrSuccess(response, gap().setInitiatorPolicyMode(mode));
    }
};



struct GetAdvertisingPolicyModeCommand : public BaseCommand {
    static const char* name() {
        return "getAdvertisingPolicyMode";
    }

    static const char* help() {
        return "Get the advertising policy filter mode that will be used in the next"
               "call to startAdvertising()";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getAdvertisingPolicyMode());
    }
};


struct GetScanningPolicyModeCommand : public BaseCommand {
    static const char* name() {
        return "getScanningPolicyMode";
    }

    static const char* help() {
        return "Get the scan policy filter mode that will be used in the next"
               "call to startScan().";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getScanningPolicyMode());
    }
};


struct GetInitiatorPolicyModeCommand : public BaseCommand {
    static const char* name() {
        return "getInitiatorPolicyMode";
    }

    static const char* help() {
        return "Get the initiator policy filter mode that will be used.";
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getInitiatorPolicyMode());
    }
};


} // end of annonymous namespace


ConstArray<const Command*> GapCommandSuiteDescription::commands() {
    static const Command* commandHandlers[] = {
        &CommandAccessor<SetAddressCommand>::command,
        &CommandAccessor<GetAddressCommand>::command,
        &CommandAccessor<GetMinAdvertisingIntervalCommand>::command,
        &CommandAccessor<GetMinNonConnectableAdvertisingIntervalCommand>::command,
        &CommandAccessor<GetMaxAdvertisingIntervalCommand>::command,
        &CommandAccessor<StopAdvertisingCommand>::command,
        &CommandAccessor<StopScanCommand>::command,
        &CommandAccessor<ConnectCommand>::command,
        &CommandAccessor<WaitForConnectionCommand>::command,
        &CommandAccessor<DisconnectCommand>::command,
        &CommandAccessor<GetPreferredConnectionParamsCommand>::command,
        &CommandAccessor<SetPreferredConnectionParamsCommand>::command,
        &CommandAccessor<UpdateConnectionParamsCommand>::command,
        &CommandAccessor<SetDeviceNameCommand>::command,
        &CommandAccessor<GetDeviceNameCommand>::command,
        &CommandAccessor<SetAppearanceCommand>::command,
        &CommandAccessor<GetAppearanceCommand>::command,
        &CommandAccessor<SetTxPowerCommand>::command,
        &CommandAccessor<GetPermittedTxPowerValuesCommand>::command,
        &CommandAccessor<GetStateCommand>::command,
        &CommandAccessor<SetAdvertisingTypeCommand>::command,
        &CommandAccessor<SetAdvertisingIntervalCommand>::command,
        &CommandAccessor<SetAdvertisingTimeoutCommand>::command,
        &CommandAccessor<StartAdvertisingCommand>::command,
        &CommandAccessor<ClearAdvertisingPayloadCommand>::command,
        &CommandAccessor<AccumulateAdvertisingPayloadCommand>::command,
        &CommandAccessor<UpdateAdvertisingPayloadCommand>::command,
        &CommandAccessor<SetAdvertisingPayloadCommand>::command,
        &CommandAccessor<GetAdvertisingPayloadCommand>::command,
        &CommandAccessor<AccumulateScanResponseCommand>::command,
        &CommandAccessor<ClearScanResponseCommand>::command,
        &CommandAccessor<SetScanParamsCommand>::command,
        &CommandAccessor<SetScanIntervalCommand>::command,
        &CommandAccessor<SetScanWindowCommand>::command,
        &CommandAccessor<SetScanTimeoutCommand>::command,
        &CommandAccessor<SetActiveScanningCommand>::command,
        &CommandAccessor<StartScanCommand>::command,
        &CommandAccessor<InitRadioNotificationCommand>::command,
        &CommandAccessor<GetAdvertisingParamsCommand>::command,
        &CommandAccessor<SetAdvertisingParamsCommand>::command,
        &CommandAccessor<GetMaxWhitelistSizeCommand>::command,
        &CommandAccessor<GetWhitelistCommand>::command,
        &CommandAccessor<SetWhitelistCommand>::command,
        &CommandAccessor<SetAdvertisingPolicyModeCommand>::command,
        &CommandAccessor<SetScanningPolicyModeCommand>::command,
        &CommandAccessor<SetInitiatorPolicyModeCommand>::command,
        &CommandAccessor<GetAdvertisingPolicyModeCommand>::command,
        &CommandAccessor<GetScanningPolicyModeCommand>::command,
        &CommandAccessor<GetInitiatorPolicyModeCommand>::command
    };

    return ConstArray<const Command*>(commandHandlers);
}
