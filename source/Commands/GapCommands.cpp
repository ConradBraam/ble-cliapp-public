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


struct SetAddressCommand : public Command {
    virtual const char* name() const {
        return "setAddress";
    }

    virtual const char* help() const {
        return "set the address of this device";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<addressType>", "The address type to set. It is a string representation of BLEProtocol::AddressType_t" },
            { "<address>"    , "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\"" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct GetAddressCommand : public Command {
    virtual const char* name() const {
        return "getAddress";
    }

    virtual const char* help() const {
        return "Get the address and the type of address of this device.\r\n"
               "The result will be a json object containing:\r\n"
               "   * 'address_type': <type of the address. It is a string representation of BLEProtocol::AddressType_t>\r\n"
               "   * 'address'     : <the address which is a string representation like 'XX:XX:XX:XX:XX:XX'>\r\n";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
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


struct GetMinAdvertisingIntervalCommand : public Command {
    virtual const char* name() const {
        return "getMinAdvertisingInterval";
    }

    virtual const char* help() const {
        return "Return the minimum advertising interval";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getMinNonConnectableAdvertisingInterval());
    }
};


struct GetMinNonConnectableAdvertisingIntervalCommand : public Command {
    virtual const char* name() const {
        return "getMinNonConnectableAdvertisingInterval";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getMinNonConnectableAdvertisingInterval());
    }
};


struct GetMaxAdvertisingIntervalCommand : public Command {
    virtual const char* name() const {
        return "getMaxAdvertisingInterval";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getMaxAdvertisingInterval());
    }
};


struct StopAdvertisingCommand : public Command {
    virtual const char* name() const {
        return "stopAdvertising";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        ble_error_t err = gap().stopAdvertising();
        reportErrorOrSuccess(response, err);
    }
};


struct StopScanCommand : public Command {
    virtual const char* name() const {
        return "stopScan";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        ble_error_t err = gap().stopScan();
        reportErrorOrSuccess(response, err);
    }
};


struct ConnectCommand : public Command {
    virtual const char* name() const {
        return "connect";
    }

    virtual const char* help() const {
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

    virtual ConstArray<CommandArgDescription> argsDescription() const {
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

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct WaitForConnectionCommand : public Command {
    virtual const char* name() const {
        return "waitForConnection";
    }

    virtual const char* help() const {
        return "Wait for a connection to occur";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<timeout>", "Maximum time allowed for this procedure" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct DisconnectCommand : public Command {
    virtual const char* name() const {
        return "disconnect";
    }

    virtual const char* help() const {
        return "disconnect the device from a specific connection.\r\n"\
               "If procedure succeed, a JSON object containing the following fields will be returned:\r\n"\
               "\t* handle: The handle disconnected\r\n"\
               "\t* reason: The reason of the disconnection (see Gap::DisconnectionReason_t)\r\n"\
               "In case of error, the reason of the error will be returned.";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The id of the connection to terminate." },
            { "<reason>", "The reason of the termination (see Gap::DisconnectionReason_t)" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct GetPreferredConnectionParamsCommand : public Command {
    virtual const char* name() const {
        return "getPreferredConnectionParams";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        Gap::ConnectionParams_t connectionParameters;
        ble_error_t err = gap().getPreferredConnectionParams(&connectionParameters);
        reportErrorOrSuccess(response, err, connectionParameters);
    }
};


struct SetPreferredConnectionParamsCommand : public Command {
    virtual const char* name() const {
        return "setPreferredConnectionParams";
    }

    virtual const char* help() const {
        return "set the prefered connection parameters";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>", "all the parameters, coma separated" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct UpdateConnectionParamsCommand : public Command {
    virtual const char* name() const {
        return "updateConnectionParams";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->notImplemented();
    }
};


struct SetDeviceNameCommand : public Command {
    virtual const char* name() const {
        return "setDeviceName";
    }

    virtual const char* help() const {
        return "set the devce name";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<name>", "the name of the device, it should not have space" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        ble_error_t err = gap().setDeviceName((const uint8_t*) args[0]);
        reportErrorOrSuccess(response, err);
    }
};


struct GetDeviceNameCommand : public Command {
    virtual const char* name() const {
        return "getDeviceName";
    }

    virtual const char* help() const {
        return "return the device name as a string";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
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


struct SetAppearanceCommand : public Command {
    virtual const char* name() const {
        return "setAppearance";
    }

    virtual const char* help() const {
        return "set the appearance flag of the device";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<appearance>", "the appearance to set (see GapAdvertisingData::Appearance_t)" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        if(!fromString(args[0], appearance)) {
            response->invalidParameters("the appearance to set is illformed (see GapAdvertisingData::Appearance_t)");
            return;
        }

        ble_error_t err = gap().setAppearance(appearance);
        reportErrorOrSuccess(response, err);
    }
};


struct GetAppearanceCommand : public Command {
    virtual const char* name() const {
        return "getAppearance";
    }

    virtual const char* help() const {
        return "get the appearance of the device";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        ble_error_t err = gap().getAppearance(&appearance);
        reportErrorOrSuccess(response, err, appearance);
    }
};


struct SetTxPowerCommand : public Command {
    virtual const char* name() const {
        return "setTxPower";
    }

    virtual const char* help() const {
        return "set the transmission power of the device";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<TxPower>", "The transmission power, it is an integer between [-128:127]"}
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        int8_t txPower = 0;
        if(!fromString(args[0], txPower)) {
            response->invalidParameters("the txPower is malformed (should be between [-127:128])");
            return;
        }

        ble_error_t err = gap().setTxPower(txPower);
        reportErrorOrSuccess(response, err);
    }
};


struct GetPermittedTxPowerValuesCommand : public Command {
    virtual const char* name() const {
        return "getPermittedTxPowerValues";
    }

    virtual const char* help() const {
        return "return an array of the authorized Tx power values";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
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


struct GetStateCommand : public Command {
    virtual const char* name() const {
        return "getState";
    }

    virtual const char* help() const {
        return "return the state of the device as defined in Gap::GapState_t";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getState());
    }
};


struct SetAdvertisingTypeCommand : public Command {
    virtual const char* name() const {
        return "setAdvertisingType";
    }

    virtual const char* help() const {
        return "set the advertising type";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<advertisingType>", "The advertising type as defined in GapAdvertisingParams::AdvertisingType_t" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        GapAdvertisingParams::AdvertisingType_t advType;

        if(!fromString(args[0], advType)) {
            response->invalidParameters("the advertising type is incorrect (see GapAdvertisingParams::AdvertisingType_t)");
            return;
        }

        gap().setAdvertisingType(advType);
        response->success();
    }
};


struct SetAdvertisingIntervalCommand : public Command {
    virtual const char* name() const {
        return "setAdvertisingInterval";
    }

    virtual const char* help() const {
        return "set the advertising interval";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<interval>", "The interval in ms, if 0, the advertising is disabled" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            response->invalidParameters("the advertising interval is ill formed");
            return;
        }

        gap().setAdvertisingInterval(interval);
        response->success();
    }
};


struct SetAdvertisingTimeoutCommand : public Command {
    virtual const char* name() const {
        return "setAdvertisingTimeout";
    }

    virtual const char* help() const {
        return "set the advertising timeout, in seconds";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<timeout>", "An integer wich represent the advertising timeout in seconds [0x1 : 0x3FFF]. 0 disable the timeout" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            response->invalidParameters("the advertising timeout is ill formed");
            return;
        }

        gap().setAdvertisingTimeout(timeout);
        response->success();
    }
};


struct StartAdvertisingCommand : public Command {
    virtual const char* name() const {
        return "startAdvertising";
    }

    virtual const char* help() const {
        return "start the advertising";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        ble_error_t err = gap().startAdvertising();
        reportErrorOrSuccess(response, err);
    }
};


struct ClearAdvertisingPayloadCommand : public Command {
    virtual const char* name() const {
        return "clearAdvertisingPayload";
    }

    virtual const char* help() const {
        return "clear the advertising payload";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        gap().clearAdvertisingPayload();
        response->success();
    }
};


struct AccumulateAdvertisingPayloadCommand : public Command {
    virtual const char* name() const {
        return "accumulateAdvertisingPayload";
    }

    virtual const char* help() const {
        return "add a new field into the advertising payload";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<fieldType>", "the field type of the data following (see GapAdvertisingData::DataType_t)" },
            { "<data>", "the value of the field, please see GapAdvertisingData" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual std::size_t maximumArgsRequired() const {
        return 0xFF;
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct UpdateAdvertisingPayloadCommand : public Command {
    virtual const char* name() const {
        return "updateAdvertisingPayload";
    }

    virtual const char* help() const {
        return "update a field int the advertising payload.\r\n"
               "Take care, at the moment, this will only succeed if the new value has the same size as the old one";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
            { "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual std::size_t maximumArgsRequired() const {
        return 0xFF;
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct SetAdvertisingPayloadCommand : public Command {
    virtual const char* name() const {
        return "setAdvertisingPayload";
    }

    virtual const char* help() const {
        return "set the advertising payload";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        // TODO
        //ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
        response->notImplemented();
    }
};


struct GetAdvertisingPayloadCommand : public Command {
    virtual const char* name() const {
        return "getAdvertisingPayload";
    }

    virtual const char* help() const {
        return "get the current advertising payload";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getAdvertisingPayload());
    }
};


struct AccumulateScanResponseCommand : public Command {
    virtual const char* name() const {
        return "accumulateScanResponse";
    }

    virtual const char* help() const {
        return "add a field into the scan response payload";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
            { "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual std::size_t maximumArgsRequired() const {
        return 0xFF;
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct ClearScanResponseCommand : public Command {
    virtual const char* name() const {
        return "clearScanResponse";
    }

    virtual const char* help() const {
        return "clear the scan response";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        gap().clearScanResponse();
        response->success();
    }
};


struct SetScanParamsCommand : public Command {
    virtual const char* name() const {
        return "setScanParams";
    }

    virtual const char* help() const {
        return "Set the scan parameters";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<interval>", "The scan interval, it should be a number between 3 and 10420ms" },
            { "<window>", "The scan window, it should be a number between 3 and 10420ms" },
            { "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " },
            { "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct SetScanIntervalCommand : public Command {
    virtual const char* name() const {
        return "setScanInterval";
    }

    virtual const char* help() const {
        return "set the scan interval";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<interval>", "The interval between each scan, it should be a number between 3 and 10240ms" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            response->invalidParameters("the interval is ill formed");
            return;
        }

        ble_error_t err = gap().setScanInterval(interval);
        reportErrorOrSuccess(response, err);
    }
};


struct SetScanWindowCommand : public Command {
    virtual const char* name() const {
        return "setScanWindow";
    }

    virtual const char* help() const {
        return "set the scan windows";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<window>", "The interval between each scan, it should be a number between 3 and 10240ms" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        uint16_t window = 0;
        if(!fromString(args[0], window)) {
            response->invalidParameters("the window is ill formed");
            return;
        }

        ble_error_t err = gap().setScanWindow(window);
        reportErrorOrSuccess(response, err);
    }
};


struct SetScanTimeoutCommand : public Command {
    virtual const char* name() const {
        return "setScanTimeout";
    }

    virtual const char* help() const {
        return "Set the scane timeout";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            response->invalidParameters("the timeout is ill formed");
            return;
        }

        ble_error_t err = gap().setScanTimeout(timeout);
        reportErrorOrSuccess(response, err);
    }
};


struct SetActiveScanningCommand : public Command {
    virtual const char* name() const {
        return "setActiveScanning";
    }

    virtual const char* help() const {
        return "Enable or disable active scanning";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        bool activeScanning = 0;
        if(!fromString(args[0], activeScanning)) {
            response->invalidParameters("the active scanning state is ill formed");
            return;
        }

        ble_error_t err = gap().setActiveScanning(activeScanning);
        reportErrorOrSuccess(response, err);
    }
};


struct StartScanCommand : public Command {
    virtual const char* name() const {
        return "startScan";
    }

    virtual const char* help() const {
        return "start the scan process";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<duration>", "The duration of the scan" },
            { "<address>", "The address to scan for" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct InitRadioNotificationCommand : public Command {
    virtual const char* name() const {
        return "initRadioNotification";
    }

    virtual const char* help() const {
        return "initialize radio notifications";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        // TODO (maybe)
        //ble_error_t initRadioNotification(void)
        response->notImplemented();
    }
};


struct GetAdvertisingParamsCommand : public Command {
    virtual const char* name() const {
        return "getAdvertisingParams";
    }

    virtual const char* help() const {
        return "return the current advertising params";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getAdvertisingParams());
    }
};


struct SetAdvertisingParamsCommand : public Command {
    virtual const char* name() const {
        return "setAdvertisingParams";
    }

    virtual const char* help() const {
        return "set the advertising parameters";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<advertisingType>", "The Advertising type, please refer to GapAdvertisingParams::AdvertisingType_t" },
            { "<interval>", "The advertising interval, it should be a number between 0 and 65534" },
            { "<timeout>", "The timeout, it should be a number between 0 and 65534" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct GetMaxWhitelistSizeCommand : public Command {
    virtual const char* name() const {
        return "getMaxWhitelistSize";
    }

    virtual const char* help() const {
        return "get the maximum size the whitelist can take";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getMaxWhitelistSize());
    }
};


struct GetWhitelistCommand : public Command {
    virtual const char* name() const {
        return "getWhitelist";
    }

    virtual const char* help() const {
        return "Get the internal whitelist to be used by the Link Layer when scanning,"
               "advertising or initiating a connection depending on the filter policies.";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
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


struct SetWhitelistCommand : public Command {
    virtual const char* name() const {
        return "setWhitelist";
    }

    virtual const char* help() const {
        return "Set the internal whitelist to be used by the Link Layer when scanning,"
               "advertising or initiating a connection depending on the filter policies.";
    }

    virtual std::size_t maximumArgsRequired() const {
        return 0xFF;
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
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


struct SetAdvertisingPolicyModeCommand : public Command {
    virtual const char* name() const {
        return "setAdvertisingPolicyMode";
    }

    virtual const char* help() const {
        return "Set the advertising policy filter mode to be used in the next call"
               "to startAdvertising().";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<AdvertisingPolicyMode_t>", "The advertising policy mode to set" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        Gap::AdvertisingPolicyMode_t mode;
        if(!fromString(args[0], mode)) {
            response->invalidParameters("The policy mode in input is ill formed");
            return;
        }

        reportErrorOrSuccess(response, gap().setAdvertisingPolicyMode(mode));
    }
};


struct SetScanningPolicyModeCommand : public Command {
    virtual const char* name() const {
        return "setScanningPolicyMode";
    }

    virtual const char* help() const {
        return "Set the scan policy filter mode to be used in the next call to startScan().";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<ScanningPolicyMode_t>", "The scanning policy mode to set" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        Gap::ScanningPolicyMode_t mode;
        if(!fromString(args[0], mode)) {
            response->invalidParameters("The policy mode in input is ill formed");
            return;
        }

        reportErrorOrSuccess(response, gap().setScanningPolicyMode(mode));
    }
};


struct SetInitiatorPolicyModeCommand : public Command {
    virtual const char* name() const {
        return "setInitiatorPolicyMode";
    }

    virtual const char* help() const {
        return "Set the initiator policy filter mode to be used.";
    }

    virtual ConstArray<CommandArgDescription> argsDescription() const {
        static const CommandArgDescription argsDescription[] = {
            { "<InitiatorPolicyMode_t>", "The scanning policy mode to set" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    virtual void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) const {
        Gap::InitiatorPolicyMode_t mode;
        if(!fromString(args[0], mode)) {
            response->invalidParameters("The policy mode in input is ill formed");
            return;
        }

        reportErrorOrSuccess(response, gap().setInitiatorPolicyMode(mode));
    }
};



struct GetAdvertisingPolicyModeCommand : public Command {
    virtual const char* name() const {
        return "getAdvertisingPolicyMode";
    }

    virtual const char* help() const {
        return "Get the advertising policy filter mode that will be used in the next"
               "call to startAdvertising()";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getAdvertisingPolicyMode());
    }
};


struct GetScanningPolicyModeCommand : public Command {
    virtual const char* name() const {
        return "getScanningPolicyMode";
    }

    virtual const char* help() const {
        return "Get the scan policy filter mode that will be used in the next"
               "call to startScan().";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getScanningPolicyMode());
    }
};


struct GetInitiatorPolicyModeCommand : public Command {
    virtual const char* name() const {
        return "getInitiatorPolicyMode";
    }

    virtual const char* help() const {
        return "Get the initiator policy filter mode that will be used.";
    }

    virtual void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) const {
        response->success(gap().getInitiatorPolicyMode());
    }
};


} // end of annonymous namespace


ConstArray<CommandAccessor_t> GapCommandSuiteDescription::commands() {
    static const CommandAccessor_t commandHandlers[] = {
        &getCommand<SetAddressCommand>,
        &getCommand<GetAddressCommand>,
        &getCommand<GetMinAdvertisingIntervalCommand>,
        &getCommand<GetMinNonConnectableAdvertisingIntervalCommand>,
        &getCommand<GetMaxAdvertisingIntervalCommand>,
        &getCommand<StopAdvertisingCommand>,
        &getCommand<StopScanCommand>,
        &getCommand<ConnectCommand>,
        &getCommand<WaitForConnectionCommand>,
        &getCommand<DisconnectCommand>,
        &getCommand<GetPreferredConnectionParamsCommand>,
        &getCommand<SetPreferredConnectionParamsCommand>,
        &getCommand<UpdateConnectionParamsCommand>,
        &getCommand<SetDeviceNameCommand>,
        &getCommand<GetDeviceNameCommand>,
        &getCommand<SetAppearanceCommand>,
        &getCommand<GetAppearanceCommand>,
        &getCommand<SetTxPowerCommand>,
        &getCommand<GetPermittedTxPowerValuesCommand>,
        &getCommand<GetStateCommand>,
        &getCommand<SetAdvertisingTypeCommand>,
        &getCommand<SetAdvertisingIntervalCommand>,
        &getCommand<SetAdvertisingTimeoutCommand>,
        &getCommand<StartAdvertisingCommand>,
        &getCommand<ClearAdvertisingPayloadCommand>,
        &getCommand<AccumulateAdvertisingPayloadCommand>,
        &getCommand<UpdateAdvertisingPayloadCommand>,
        &getCommand<SetAdvertisingPayloadCommand>,
        &getCommand<GetAdvertisingPayloadCommand>,
        &getCommand<AccumulateScanResponseCommand>,
        &getCommand<ClearScanResponseCommand>,
        &getCommand<SetScanParamsCommand>,
        &getCommand<SetScanIntervalCommand>,
        &getCommand<SetScanWindowCommand>,
        &getCommand<SetScanTimeoutCommand>,
        &getCommand<SetActiveScanningCommand>,
        &getCommand<StartScanCommand>,
        &getCommand<InitRadioNotificationCommand>,
        &getCommand<GetAdvertisingParamsCommand>,
        &getCommand<SetAdvertisingParamsCommand>,
        &getCommand<GetMaxWhitelistSizeCommand>,
        &getCommand<GetWhitelistCommand>,
        &getCommand<SetWhitelistCommand>,
        &getCommand<SetAdvertisingPolicyModeCommand>,
        &getCommand<SetScanningPolicyModeCommand>,
        &getCommand<SetInitiatorPolicyModeCommand>,
        &getCommand<GetAdvertisingPolicyModeCommand>,
        &getCommand<GetScanningPolicyModeCommand>,
        &getCommand<GetInitiatorPolicyModeCommand>
    };

    return ConstArray<CommandAccessor_t>(commandHandlers);
}
