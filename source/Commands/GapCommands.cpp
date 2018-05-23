

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

using mbed::util::SharedPointer;

// isolation ...
namespace {


DECLARE_CMD(SetAddressCommand) {
    CMD_NAME("setAddress")
    CMD_HELP("set the address of this device")
    CMD_ARGS(
        CMD_ARG("BLEProtocol::AddressType_t", "addressType", "The type of the address to set."),
        CMD_ARG("MacAddress_t", "address" , "The address to set")
    )
    CMD_HANDLER(BLEProtocol::AddressType_t addressType, MacAddress_t address, CommandResponsePtr& response) {
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


DECLARE_CMD(StopAdvertisingCommand) {
    CMD_NAME("stopAdvertising")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().stopAdvertising();
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(StopScanCommand) {
    CMD_NAME("stopScan")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().stopScan();
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(ConnectCommand) {
    CMD_NAME("connect")
    CMD_HELP( "connect to a device, if this function succeed, a ConnectionCallbackParams_t is returned.")

    CMD_ARGS(
        CMD_ARG("BLEProtocol::AddressType_t", "addressType", "The address type to of the peer device."),
        CMD_ARG("MacAddress_t", "address", "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\""),
            // connection parameters
        CMD_ARG("uint16_t", "minConnectionInterval", "Minimum Connection Interval in 1.25 ms units"),
        CMD_ARG("uint16_t", "maxConnectionInterval", "Maximum Connection Interval in 1.25 ms units"),
        CMD_ARG("uint16_t", "slaveLatency", "Slave Latency in number of connection events"),
        CMD_ARG("uint16_t", "connectionSupervisionTimeout", "Connection Supervision Timeout in 10 ms units"),
            // scan parameters
        CMD_ARG("uint16_t", "interval", "The scan interval, it should be a number between 3 and 10420ms."),
        CMD_ARG("uint16_t", "window", "The scan window, it should be a number between 3 and 10420ms."),
        CMD_ARG("uint16_t", "scan timeout", "The scan timeout, it should be a number between 0 and 65534"),
        CMD_ARG("bool", "activeScanning", "A boolean value { true, false } indicating if the device send scan request or not"),
            // timeout for this procedure
        CMD_ARG("uint16_t", "timeout", "Maximum time allowed for this procedure")
    )

    CMD_RESULTS(
        CMD_RESULT("uint16_t", "handle", "The handle of the connection created"),
        CMD_RESULT("Gap::Role", "role", "Role of the device in the connection (here, it should be central)"),
        CMD_RESULT("BLEProtocol::AddressType_t", "peerAddrType", "The addressType of the peer"),
        CMD_RESULT("MacAddress_t", "peerAddr", "The address of the peer"),
        CMD_RESULT("BLEProtocol::AddressType_t", "ownAddrType", "The address type of this device"),
        CMD_RESULT("MacAddress_t", "ownAddr", "The address of this device"),
        CMD_RESULT("JSON object", "connectionParams", "Object which contain the parameters of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.minConnectionInterval", "minimum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.maxConnectionInterval", "maximum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.slaveLatency", "slave latency of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.connectionSupervisionTimeout", "supervision timeout for this connection")
    )

    CMD_HANDLER(BLEProtocol::AddressType_t addressType, MacAddress_t address, uint16_t minConnectionInterval, uint16_t maxConnectionInterval,
                uint16_t slaveLatency, uint16_t connectionSupervisionTimeout, uint16_t scanInterval, uint16_t window, uint16_t scanTimeout,
                bool activeScanning, uint16_t procedureTimeout, CommandResponsePtr& response) {
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


DECLARE_CMD(WaitForConnectionCommand) {
    CMD_NAME("waitForConnection")
    CMD_HELP("Wait for a connection to occur. If another device connect, then the command will return the connection parameters.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "timeout", "Maximum time allowed for this procedure")
    )

    CMD_RESULTS(
        CMD_RESULT("uint16_t", "handle", "The handle of the connection created"),
        CMD_RESULT("Gap::Role", "role", "Role of the device in the connection (here, it should be peripheral)"),
        CMD_RESULT("BLEProtocol::AddressType_t", "peerAddrType", "The addressType of the peer"),
        CMD_RESULT("MacAddress_t", "peerAddr", "The address of the peer"),
        CMD_RESULT("BLEProtocol::AddressType_t", "ownAddrType", "The address type of this device"),
        CMD_RESULT("MacAddress_t", "ownAddr", "The address of this device"),
        CMD_RESULT("JSON object", "connectionParams", "Object which contain the parameters of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.minConnectionInterval", "minimum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.maxConnectionInterval", "maximum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.slaveLatency", "slave latency of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.connectionSupervisionTimeout", "supervision timeout for this connection")
    )


    CMD_HANDLER(uint16_t timeout, CommandResponsePtr& response) {
        startProcedure<WaitForConnectionProcedure>(response, timeout);
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


DECLARE_CMD(DisconnectCommand) {
    CMD_NAME("disconnect")
    CMD_HELP("disconnect the device from a specific connection."\
        "If the procedure succeed, a JSON object containing the following fields will be returned."\
        "In case of error, the reason of the error will be returned."
    )

    CMD_ARGS(
        CMD_ARG("Gap::Handle_t", "connectionHandle", "The id of the connection to terminate."),
        CMD_ARG("Gap::DisconnectionReason_t", "reason", "The reason of the termination (see Gap::DisconnectionReason_t)")
    )

    CMD_RESULTS(
        CMD_RESULT("uint16_t", "handle", "The connection handle disconnected"),
        CMD_RESULT("Gap::DisconnectionReason_t", "reason", "The reason of the disconnection")
    )

    CMD_HANDLER(Gap::Handle_t connectionHandle, Gap::DisconnectionReason_t reason, CommandResponsePtr& response) {
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


DECLARE_CMD(GetPreferredConnectionParamsCommand) {
    CMD_NAME("getPreferredConnectionParams")
    CMD_HANDLER(CommandResponsePtr& response) {
        Gap::ConnectionParams_t connectionParameters;
        ble_error_t err = gap().getPreferredConnectionParams(&connectionParameters);
        reportErrorOrSuccess(response, err, connectionParameters);
    }
};


DECLARE_CMD(SetPreferredConnectionParamsCommand) {
    CMD_NAME("setPreferredConnectionParams")
    CMD_HELP("set the prefered connection parameters")

    CMD_ARGS(
        CMD_ARG(
            "Gap::ConnectionParams_t",
            "minConnectionInterval,maxConnectionInterval,slaveLatency,connectionSupervisionTimeout",
            "all the parameters, coma separated"
        )
    )

    CMD_HANDLER(Gap::ConnectionParams_t connectionParameters, CommandResponsePtr& response) {
        ble_error_t err =  gap().setPreferredConnectionParams(&connectionParameters);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(UpdateConnectionParamsCommand) {
    CMD_NAME("updateConnectionParams")
    CMD_HANDLER(CommandResponsePtr& response) {
        response->notImplemented();
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


DECLARE_CMD(SetTxPowerCommand) {
    CMD_NAME("setTxPower")
    CMD_HELP("set the transmission power of the device")

    CMD_ARGS(
        CMD_ARG("int8_t", "TxPower", "The transmission power, it is an integer between [-128:127]")
    )

    CMD_HANDLER(int8_t txPower, CommandResponsePtr& response) {
        ble_error_t err = gap().setTxPower(txPower);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(GetPermittedTxPowerValuesCommand) {
    CMD_NAME("getPermittedTxPowerValues")
    CMD_HELP("return an array of the authorized Tx power values")

    CMD_HANDLER(CommandResponsePtr& response) {
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


DECLARE_CMD(GetStateCommand) {
    CMD_NAME("getState")
    CMD_HELP("return the state of the device as defined in Gap::GapState_t")

    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getState());
    }
};


DECLARE_CMD(SetAdvertisingTypeCommand) {
    CMD_NAME("setAdvertisingType")
    CMD_HELP("set the advertising type")
    CMD_ARGS(
        CMD_ARG("GapAdvertisingParams::AdvertisingType_t", "advertisingType", "The advertising type to set")
    )
    CMD_HANDLER(GapAdvertisingParams::AdvertisingType_t advType, CommandResponsePtr& response) {
        gap().setAdvertisingType(advType);
        response->success();
    }
};


DECLARE_CMD(SetAdvertisingIntervalCommand) {
    CMD_NAME("setAdvertisingInterval")
    CMD_HELP("set the advertising interval")

    CMD_ARGS(
        CMD_ARG("uint16_t", "interval", "The interval in ms, if 0, the advertising is disabled")
    )

    CMD_HANDLER(uint16_t interval, CommandResponsePtr& response) {
        gap().setAdvertisingInterval(interval);
        response->success();
    }
};


DECLARE_CMD(SetAdvertisingTimeoutCommand) {
    CMD_NAME("setAdvertisingTimeout")
    CMD_HELP("set the advertising timeout, in seconds")

    CMD_ARGS(
        CMD_ARG("uint16_t", "timeout", "An integer wich represent the advertising timeout in seconds [0x1 : 0x3FFF]. 0 disable the timeout")
    )

    CMD_HANDLER(uint16_t timeout, CommandResponsePtr& response) {
        gap().setAdvertisingTimeout(timeout);
        response->success();
    }
};


DECLARE_CMD(StartAdvertisingCommand) {
    CMD_NAME("startAdvertising")
    CMD_HELP("start the advertising")

    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().startAdvertising();
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(ClearAdvertisingPayloadCommand) {
    CMD_NAME("clearAdvertisingPayload")
    CMD_HELP("clear the advertising payload")

    CMD_HANDLER(CommandResponsePtr& response) {
        gap().clearAdvertisingPayload();
        response->success();
    }
};


DECLARE_CMD(AccumulateAdvertisingPayloadCommand) {
    CMD_NAME("accumulateAdvertisingPayload")
    CMD_HELP("add a new field into the advertising payload")

    CMD_ARGS(
        CMD_ARG("AdvertisingPayloadField_t", "fieldType", "the field type of the data following (see GapAdvertisingData::DataType_t)"),
        CMD_ARG("undefined", "data", "the value of the field, please see GapAdvertisingData")
    )

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
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


DECLARE_CMD(UpdateAdvertisingPayloadCommand) {
    CMD_NAME("updateAdvertisingPayload")
    CMD_HELP("update a field int the advertising payload."
               "Take care, at the moment, this will only succeed if the new value has the same size as the old one")

    CMD_ARGS(
        CMD_ARG("AdvertisingPayloadField_t", "fieldType", "the field type to update (see GapAdvertisingData::DataType_t)"),
        CMD_ARG("undefined", "data", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData")
    )

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
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


DECLARE_CMD(SetAdvertisingPayloadCommand) {
    CMD_NAME("setAdvertisingPayload")
    CMD_HELP("set the advertising payload")

    CMD_HANDLER(CommandResponsePtr& response) {
        // TODO
        //ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
        response->notImplemented();
    }
};


DECLARE_CMD(GetAdvertisingPayloadCommand) {
    CMD_NAME("getAdvertisingPayload")
    CMD_HELP("get the current advertising payload")

    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getAdvertisingPayload());
    }
};


DECLARE_CMD(AccumulateScanResponseCommand) {
    CMD_NAME("accumulateScanResponse")
    CMD_HELP("add a field into the scan response payload")

    CMD_ARGS(
        CMD_ARG("AdvertisingPayloadField_t", "fieldType", "the field type to update (see GapAdvertisingData::DataType_t)"),
        CMD_ARG("undefined", "data", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData")
    )

    template<typename T>
    static std::size_t maximumArgsRequired() {
        return 0xFF;
    }

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
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


DECLARE_CMD(ClearScanResponseCommand) {
    CMD_NAME("clearScanResponse")
    CMD_HELP("clear the scan response")

    CMD_HANDLER(CommandResponsePtr& response) {
        gap().clearScanResponse();
        response->success();
    }
};


DECLARE_CMD(SetScanParamsCommand) {
    CMD_NAME("setScanParams")
    CMD_HELP("Set the scan parameters")

    CMD_ARGS(
        CMD_ARG("uint16_t", "interval", "The scan interval, it should be a number between 3 and 10420ms"),
        CMD_ARG("uint16_t", "window", "The scan window, it should be a number between 3 and 10420ms"),
        CMD_ARG("uint16_t", "timeout", "The scan timeout, it should be a number between 0 and 65534 "),
        CMD_ARG("uint16_t", "activeScanning", "A boolean value { true, false } indeicating if the device send scan request or not")
    )

    CMD_HANDLER(uint16_t interval, uint16_t window, uint16_t timeout, uint16_t activeScanning, CommandResponsePtr& response) {
        ble_error_t err = gap().setScanParams(interval, window, timeout, activeScanning);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(SetScanIntervalCommand) {
    CMD_NAME("setScanInterval")
    CMD_HELP("set the scan interval")

    CMD_ARGS(
        CMD_ARG("uint16_t", "interval", "The interval between each scan, it should be a number between 3 and 10240ms")
    )

    CMD_HANDLER(uint16_t interval, CommandResponsePtr& response) {
        ble_error_t err = gap().setScanInterval(interval);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(SetScanWindowCommand) {
    CMD_NAME("setScanWindow")
    CMD_HELP("set the scan windows")

    CMD_ARGS(
        CMD_ARG("uint16_t", "window", "The interval between each scan, it should be a number between 3 and 10240ms")
    )

    CMD_HANDLER(uint16_t window, CommandResponsePtr& response) {
        ble_error_t err = gap().setScanWindow(window);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(SetScanTimeoutCommand) {
    CMD_NAME("setScanTimeout")
    CMD_HELP("Set the scane timeout")

    CMD_ARGS(
        CMD_ARG("uint16_t", "timeout", "The scan timeout, it should be a number between 0 and 65534 ")
    )

    CMD_HANDLER(uint16_t timeout, CommandResponsePtr& response) {
        ble_error_t err = gap().setScanTimeout(timeout);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(SetActiveScanningCommand) {
    CMD_NAME("setActiveScanning")
    CMD_HELP("Enable or disable active scanning")

    CMD_ARGS(
        CMD_ARG("uint16_t", "activeScanning", "A boolean value { true, false } indeicating if the device send scan request or not")
    )

    CMD_HANDLER(bool activeScanning, CommandResponsePtr& response) {
        ble_error_t err = gap().setActiveScanning(activeScanning);
        reportErrorOrSuccess(response, err);
    }
};


DECLARE_CMD(StartScanCommand) {
    CMD_NAME("startScan")
    CMD_HELP("start the scan process")

    CMD_ARGS(
        CMD_ARG("uint16_t", "duration", "The duration of the scan"),
        CMD_ARG("HexString_t|MacAddress_t", "payload|address", "The address or the payload to scan for")
    )

    CMD_RESULTS(
        CMD_RESULT("JSON Array", "", "Array of scan results"),
        CMD_RESULT("JSON Object", "[x]", "A scan result"),
        CMD_RESULT("MacAddress_t", "[x].peerAddr", "Address of the peer adverising."),
        CMD_RESULT("int8_t", "[x].rssi", "RSSI of the scan sample."),
        CMD_RESULT("bool", "[x].isScanResponse", "Indicate if it is an advertising or a scan response."),
        CMD_RESULT("GapAdvertisingParams::AdvertisingType_t", "[x].type", "Type of the scan result."),
        CMD_RESULT("uint32_t", "[x].time", "Time (in ms) at which the scan has been acquired since the begining of the start procedure."),
        CMD_RESULT("JSON object", "[x].data", "Object containing the different fields of the advertisement."),
        CMD_RESULT("HexString_t", "[x].data.raw", "Raw payload of the advertising.")
    )

    CMD_HANDLER(const CommandArgs& args, CommandResponsePtr& response) {
        if (args.count() != 2) {
            response->invalidParameters("2 arguments are required: startScan <duration> <address|payload>");
            return;
        }

        uint16_t duration;
        if (!fromString(args[0], duration)) {
            response->invalidParameters("duration should be an uint16_t");
        }

        MacAddress_t address;
        RawData_t payload;

        if (fromString(args[1], address)) {
            startProcedure<ScanProcedure>(response, duration, address);
        } else if(fromString(args[1], payload)) {
             startProcedure<ScanProcedure>(response, duration, payload);
        } else {
            response->invalidParameters("second parameter should be a payload or a mac address");
        }
    }

    struct ScanProcedure : public AsyncProcedure {
        ScanProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, const Gap::Address_t& addr) :
            AsyncProcedure(res, timeout), use_payload(false) {
                memcpy(address, addr, sizeof(address));
        }

        ScanProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, const RawData_t& payload) :
            AsyncProcedure(res, timeout), payload(payload), use_payload(true) {
        }

        virtual ~ScanProcedure() {
            // close the array of scan
            self = NULL;
            gap().stopScan();
            timer.stop();
            // note : there should be a way to detach this function pointer in the BLE API
        }

        virtual bool doStart() {
            if (self) {
                response->faillure("Scan procedure already started");
                return false;
            }

            self = this;
            ble_error_t err = gap().startScan(&ScanProcedure::whenAdvertisementReceived);
            if(err) {
                self = NULL;
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

        static void whenAdvertisementReceived(const Gap::AdvertisementCallbackParams_t* scanResult) {
            if (self == NULL) {
                return;
            }

            using namespace serialization;

            if (self->use_payload) {
                if (scanResult->advertisingDataLen != self->payload.size()) {
                    return;
                }
                if (memcmp(self->payload.cbegin(), scanResult->advertisingData, self->payload.size()) != 0) {
                    return;
                }
            } else {
                // check the address, if it is not the address wanted, just return
                if(memcmp(scanResult->peerAddr, self->address, sizeof(self->address))) {
                    return;
                }
            }

            self->response->getResultStream() << startObject <<
                key("peerAddr") << macAddressToString(scanResult->peerAddr).str <<
                key("rssi") << scanResult->rssi <<
                key("isScanResponse") << scanResult->isScanResponse <<
                key("type") << scanResult->type <<
                key("data") << AdvertisingDataSerializer(scanResult->advertisingData, scanResult->advertisingDataLen) <<
                key("time") << (int32_t) self->timer.read_ms() <<
                key("peerAddrType") << scanResult->addressType <<
            endObject;
        }

        virtual void doWhenTimeout() {
            response->getResultStream() << serialization::endArray;
            // nothing to do here, timeout is not an error in this case
        }

        Gap::Address_t address;
        mbed::Timer timer;
        // global object to workaround BLE API scan limitation:
        // the scan callback cannot be replaced and if the scan stop
        // asynchronously then the stack continue to emit undesired scan events
        static ScanProcedure* self;
        RawData_t payload;
        bool use_payload;
    };
};

StartScanCommand::ScanProcedure* StartScanCommand::ScanProcedure::self = NULL;


DECLARE_CMD(InitRadioNotificationCommand) {
    CMD_NAME("initRadioNotification")
    CMD_HELP("initialize radio notifications")

    CMD_HANDLER(CommandResponsePtr& response) {
        // TODO (maybe)
        //ble_error_t initRadioNotification(void)
        response->notImplemented();
    }
};


DECLARE_CMD(GetAdvertisingParamsCommand) {
    CMD_NAME("getAdvertisingParams")
    CMD_HELP("return the current advertising params")

    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getAdvertisingParams());
    }
};


DECLARE_CMD(SetAdvertisingParamsCommand) {
    CMD_NAME("setAdvertisingParams")
    CMD_HELP("set the advertising parameters")

    CMD_ARGS(
        CMD_ARG("GapAdvertisingParams::AdvertisingType_t", "advertisingType", "The Advertising type, please refer to GapAdvertisingParams::AdvertisingType_t"),
        CMD_ARG("uint16_t", "interval", "The advertising interval, it should be a number between 0 and 65534"),
        CMD_ARG("uint16_t", "timeout", "The timeout, it should be a number between 0 and 65534")
    )

    CMD_HANDLER(GapAdvertisingParams::AdvertisingType_t advertisingType, uint16_t interval, uint16_t timeout, CommandResponsePtr& response) {
        gap().setAdvertisingParams(GapAdvertisingParams(advertisingType, interval, timeout));
        response->success();
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


DECLARE_CMD(SetAdvertisingPolicyModeCommand) {
    CMD_NAME("setAdvertisingPolicyMode")
    CMD_HELP("Set the advertising policy filter mode to be used in the next call"
               "to startAdvertising().")

    CMD_ARGS(
        CMD_ARG("Gap::AdvertisingPolicyMode_t", "AdvertisingPolicyMode_t", "The advertising policy mode to set")
    )

    CMD_HANDLER(Gap::AdvertisingPolicyMode_t mode, CommandResponsePtr& response) {
        reportErrorOrSuccess(response, gap().setAdvertisingPolicyMode(mode));
    }
};


DECLARE_CMD(SetScanningPolicyModeCommand) {
    CMD_NAME("setScanningPolicyMode")
    CMD_HELP("Set the scan policy filter mode to be used in the next call to startScan().")

    CMD_ARGS(
        CMD_ARG("Gap::ScanningPolicyMode_t", "ScanningPolicyMode_t", "The scanning policy mode to set")
    )

    CMD_HANDLER(Gap::ScanningPolicyMode_t mode, CommandResponsePtr& response) {
        reportErrorOrSuccess(response, gap().setScanningPolicyMode(mode));
    }
};


DECLARE_CMD(SetInitiatorPolicyModeCommand) {
    CMD_NAME("setInitiatorPolicyMode")
    CMD_HELP("Set the initiator policy filter mode to be used.")

    CMD_ARGS(
        CMD_ARG("Gap::InitiatorPolicyMode_t", "InitiatorPolicyMode_t", "The scanning policy mode to set")
    )

    CMD_HANDLER(Gap::InitiatorPolicyMode_t mode, CommandResponsePtr& response) {
        reportErrorOrSuccess(response, gap().setInitiatorPolicyMode(mode));
    }
};



DECLARE_CMD(GetAdvertisingPolicyModeCommand) {
    CMD_NAME("getAdvertisingPolicyMode")
    CMD_HELP("Get the advertising policy filter mode that will be used in the next"
               "call to startAdvertising()")

    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getAdvertisingPolicyMode());
    }
};


DECLARE_CMD(GetScanningPolicyModeCommand) {
    CMD_NAME("getScanningPolicyMode")
    CMD_HELP("Get the scan policy filter mode that will be used in the next"
               "call to startScan().")

    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getScanningPolicyMode());
    }
};


DECLARE_CMD(GetInitiatorPolicyModeCommand) {
    CMD_NAME("getInitiatorPolicyMode")
    CMD_HELP("Get the initiator policy filter mode that will be used.")

    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getInitiatorPolicyMode());
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
        CMD_ARG("Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy", "resolution_strategy", "Strategy used to resolve addresses present in scan and connection requests.")
    )

    CMD_HANDLER(
        bool use_non_resolvable_random_address,
        Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy& resolution_strategy,
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
        CMD_RESULT("Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy", "resolution_strategy", "Strategy used to resolve address in scan and connection requests."),
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
        CMD_ARG("Gap::CentralPrivacyConfiguration_t::ResolutionStrategy", "resolution_strategy", "Strategy used to resolve addresses present in advertisement packets.")
    )

    CMD_HANDLER(
        bool use_non_resolvable_random_address,
        Gap::CentralPrivacyConfiguration_t::ResolutionStrategy& resolution_strategy,
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
        CMD_RESULT("Gap::CentralPrivacyConfiguration_t::ResolutionStrategy", "resolution_strategy", "Strategy used to resolve addresses in advertisements."),
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


} // end of annonymous namespace


DECLARE_SUITE_COMMANDS(GapCommandSuiteDescription,
    CMD_INSTANCE(SetAddressCommand),
    CMD_INSTANCE(GetAddressCommand),
    CMD_INSTANCE(GetMinAdvertisingIntervalCommand),
    CMD_INSTANCE(GetMinNonConnectableAdvertisingIntervalCommand),
    CMD_INSTANCE(GetMaxAdvertisingIntervalCommand),
    CMD_INSTANCE(StopAdvertisingCommand),
    CMD_INSTANCE(StopScanCommand),
    CMD_INSTANCE(ConnectCommand),
    CMD_INSTANCE(WaitForConnectionCommand),
    CMD_INSTANCE(DisconnectCommand),
    CMD_INSTANCE(GetPreferredConnectionParamsCommand),
    CMD_INSTANCE(SetPreferredConnectionParamsCommand),
    CMD_INSTANCE(UpdateConnectionParamsCommand),
    CMD_INSTANCE(SetDeviceNameCommand),
    CMD_INSTANCE(GetDeviceNameCommand),
    CMD_INSTANCE(SetAppearanceCommand),
    CMD_INSTANCE(GetAppearanceCommand),
    CMD_INSTANCE(SetTxPowerCommand),
    CMD_INSTANCE(GetPermittedTxPowerValuesCommand),
    CMD_INSTANCE(GetStateCommand),
    CMD_INSTANCE(SetAdvertisingTypeCommand),
    CMD_INSTANCE(SetAdvertisingIntervalCommand),
    CMD_INSTANCE(SetAdvertisingTimeoutCommand),
    CMD_INSTANCE(StartAdvertisingCommand),
    CMD_INSTANCE(ClearAdvertisingPayloadCommand),
    CMD_INSTANCE(AccumulateAdvertisingPayloadCommand),
    CMD_INSTANCE(UpdateAdvertisingPayloadCommand),
    CMD_INSTANCE(SetAdvertisingPayloadCommand),
    CMD_INSTANCE(GetAdvertisingPayloadCommand),
    CMD_INSTANCE(AccumulateScanResponseCommand),
    CMD_INSTANCE(ClearScanResponseCommand),
    CMD_INSTANCE(SetScanParamsCommand),
    CMD_INSTANCE(SetScanIntervalCommand),
    CMD_INSTANCE(SetScanWindowCommand),
    CMD_INSTANCE(SetScanTimeoutCommand),
    CMD_INSTANCE(SetActiveScanningCommand),
    CMD_INSTANCE(StartScanCommand),
    CMD_INSTANCE(InitRadioNotificationCommand),
    CMD_INSTANCE(GetAdvertisingParamsCommand),
    CMD_INSTANCE(SetAdvertisingParamsCommand),
    CMD_INSTANCE(GetMaxWhitelistSizeCommand),
    CMD_INSTANCE(GetWhitelistCommand),
    CMD_INSTANCE(SetWhitelistCommand),
    CMD_INSTANCE(SetAdvertisingPolicyModeCommand),
    CMD_INSTANCE(SetScanningPolicyModeCommand),
    CMD_INSTANCE(SetInitiatorPolicyModeCommand),
    CMD_INSTANCE(GetAdvertisingPolicyModeCommand),
    CMD_INSTANCE(GetScanningPolicyModeCommand),
    CMD_INSTANCE(GetInitiatorPolicyModeCommand),
    CMD_INSTANCE(EnablePrivacyCommand),
    CMD_INSTANCE(SetPeripheralPrivacyConfigurationCommand),
    CMD_INSTANCE(GetPeripheralPrivacyConfigurationCommand),
    CMD_INSTANCE(SetCentralPrivacyConfigurationCommand),
    CMD_INSTANCE(GetCentralPrivacyConfigurationCommand)
)
