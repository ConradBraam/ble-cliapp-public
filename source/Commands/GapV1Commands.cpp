


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

#include "GapV1Commands.h"

using mbed::util::SharedPointer;

typedef BLEProtocol::AddressType_t LegacyAddressType_t;

// isolation ...
namespace {

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
        CMD_ARG("ble::peer_address_type_t|LegacyAddresstype_t", "addressType", "The address type to of the peer device."),
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
        CMD_RESULT("LegacyAddresstype_t", "peerAddrType", "The addressType of the peer"),
        CMD_RESULT("MacAddress_t", "peerAddr", "The address of the peer"),
        CMD_RESULT("BLEProtocol::AddressType_t", "ownAddrType", "The address type of this device"),
        CMD_RESULT("MacAddress_t", "ownAddr", "The address of this device"),
        CMD_RESULT("JSON object", "connectionParams", "Object which contain the parameters of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.minConnectionInterval", "minimum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.maxConnectionInterval", "maximum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.slaveLatency", "slave latency of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.connectionSupervisionTimeout", "supervision timeout for this connection"),
        CMD_RESULT("ble::peer_address_type_t", "peerAddressType", "The addressType of the peer")
    )

    CMD_HANDLER(
        const CommandArgs& args, CommandResponsePtr& response
    ) {
        if (args.count() != 11) {
            response->invalidParameters("11 arguments are required");
            return;
        }

        bool use_legacy_address_type;

        ble::peer_address_type_t addressType;
        Gap::AddressType_t legacyAddressType;

        if (fromString(args[0], addressType)) {
            use_legacy_address_type = false;
        } else if(fromString(args[0], legacyAddressType)) {
            use_legacy_address_type = true;
        } else {
            response->invalidParameters("invalid address type");
            return;
        }

        MacAddress_t address;
        if (!fromString(args[1], address)) {
            response->invalidParameters("invalid address");
            return;
        }

        uint16_t minConnectionInterval;
        if (!fromString(args[2], minConnectionInterval)) {
            response->invalidParameters("invalid minConnectionInterval");
            return;
        }

        uint16_t maxConnectionInterval;
        if (!fromString(args[3], maxConnectionInterval)) {
            response->invalidParameters("invalid maxConnectionInterval");
            return;
        }

        uint16_t slaveLatency;
        if (!fromString(args[4], slaveLatency)) {
            response->invalidParameters("invalid slaveLatency");
            return;
        }

        uint16_t connectionSupervisionTimeout;
        if (!fromString(args[5], connectionSupervisionTimeout)) {
            response->invalidParameters("invalid connectionSupervisionTimeout");
            return;
        }

        uint16_t scanInterval;
        if (!fromString(args[6], scanInterval)) {
            response->invalidParameters("invalid scanInterval");
            return;
        }

        uint16_t window;
        if (!fromString(args[7], window)) {
            response->invalidParameters("invalid window");
            return;
        }

        uint16_t scanTimeout;
        if (!fromString(args[8], scanTimeout)) {
            response->invalidParameters("invalid scanTimeout");
            return;
        }

        bool activeScanning;
        if (!fromString(args[9], activeScanning)) {
            response->invalidParameters("invalid scanTimeout");
            return;
        }

        uint16_t procedureTimeout;
        if (!fromString(args[10], procedureTimeout)) {
            response->invalidParameters("invalid procedureTimeout");
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


        ble_error_t err;
        if (use_legacy_address_type) {
            err = gap().connect(
                address,
                legacyAddressType,
                &connectionParams,
                &scanParams
            );
        } else {
            err = gap().connect(
                address,
                addressType,
                &connectionParams,
                &scanParams
            );
        }

        if (err) {
            response->faillure(err);
            return;
        }

        startProcedure<ConnectionProcedure>(
            addressType,
            legacyAddressType,
            use_legacy_address_type,
            address,
            response,
            procedureTimeout
        );
    }

    struct ConnectionProcedure : public AsyncProcedure {
        ConnectionProcedure(
            ble::peer_address_type_t _addressType,
            Gap::AddressType_t legacyAddressType,
            bool use_legacy_address_type,
            const Gap::Address_t& _address,
            const SharedPointer<CommandResponse>& res,
            uint32_t procedureTimeout
        ) : AsyncProcedure(res, procedureTimeout),
            addressType(_addressType),
            legacyAddressType(legacyAddressType),
            use_legacy_address_type(use_legacy_address_type) {
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
                ((use_legacy_address_type == false) && (params->peerAddressType != addressType)) ||
                ((use_legacy_address_type == true) && (params->peerAddrType != legacyAddressType))
                ) {
                response->faillure();
                serialization::JSONOutputStream& os = response->getResultStream();
                os << startObject <<
                    key("expected_address") << macAddressToString(address).str <<
                    key("actual_address") << macAddressToString(params->peerAddr).str <<
                    key("expected_address_type") << toString(addressType) <<
                    key("actual_address_type") << toString(params->peerAddrType) <<
                    endObject;

                terminate();

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
                key("localResolvableAddr") << macAddressToString(params->localResolvableAddr).str <<
                key("peerResolvableAddr") << macAddressToString(params->peerResolvableAddr).str <<
                key("peerAddressType") << toString(params->peerAddressType) <<
                endObject;

            terminate();
        }

        ble::peer_address_type_t addressType;
        Gap::AddressType_t legacyAddressType;
        bool use_legacy_address_type;
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
        CMD_RESULT("LegacyAddresstype_t", "peerAddrType", "The addressType of the peer"),
        CMD_RESULT("MacAddress_t", "peerAddr", "The address of the peer"),
        CMD_RESULT("BLEProtocol::AddressType_t", "ownAddrType", "The address type of this device"),
        CMD_RESULT("MacAddress_t", "ownAddr", "The address of this device"),
        CMD_RESULT("JSON object", "connectionParams", "Object which contain the parameters of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.minConnectionInterval", "minimum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.maxConnectionInterval", "maximum connection interval for this connection"),
        CMD_RESULT("uint16_t", "connectionParams.slaveLatency", "slave latency of the connection"),
        CMD_RESULT("uint16_t", "connectionParams.connectionSupervisionTimeout", "supervision timeout for this connection"),
        CMD_RESULT("ble::peer_address_type_t", "peerAddressType", "The addressType of the peer")
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
                key("localResolvableAddr") << macAddressToString(params->localResolvableAddr).str <<
                key("peerResolvableAddr") << macAddressToString(params->peerResolvableAddr).str <<
                key("peerAddressType") << toString(params->peerAddressType) <<
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
                key("addressType") << scanResult->addressType <<
                key("peerAddrType") << scanResult->peerAddrType <<
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

} // end of annonymous namespace


DECLARE_SUITE_COMMANDS(GapV1CommandSuiteDescription,
    CMD_INSTANCE(StopAdvertisingCommand),
    CMD_INSTANCE(StopScanCommand),
    CMD_INSTANCE(ConnectCommand),
    CMD_INSTANCE(WaitForConnectionCommand),
    CMD_INSTANCE(DisconnectCommand),
    CMD_INSTANCE(GetPreferredConnectionParamsCommand),
    CMD_INSTANCE(SetPreferredConnectionParamsCommand),
    CMD_INSTANCE(UpdateConnectionParamsCommand),
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
    CMD_INSTANCE(SetAdvertisingPolicyModeCommand),
    CMD_INSTANCE(SetScanningPolicyModeCommand),
    CMD_INSTANCE(SetInitiatorPolicyModeCommand),
    CMD_INSTANCE(GetAdvertisingPolicyModeCommand),
    CMD_INSTANCE(GetScanningPolicyModeCommand),
    CMD_INSTANCE(GetInitiatorPolicyModeCommand)
)
