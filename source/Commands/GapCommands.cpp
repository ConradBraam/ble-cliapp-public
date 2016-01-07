#include "GapCommands.h"

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/GapAdvertisingDataSerializer.h"
#include "Serialization/GapAdvertisingParamsSerializer.h"
#include "Serialization/BLECommonSerializer.h"
#include "util/StaticLambda.h"
#include "minar/minar.h"
#include "CLICommand/CommandSuite.h"
#include "util/AsyncProcedure.h"

template<typename T>
using SharedPointer = mbed::util::SharedPointer<T>;

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


static constexpr const Command setAddress {
    "setAddress",
    "set the address of this device",
    (const CommandArgDescription[]) {
        { "<addressType>", "The address type to set. It is a string representation of BLEProtocol::AddressType_t" },
        { "<address>"    , "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\"" },
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

        auto err = gap().setAddress(addressType, address);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command getAddress {
    "getAddress",
    "Get the address and the type of address of this device.\r\n"
    "The result will be a json object containing:\r\n"
    "   * 'address_type': <type of the address. It is a string representation of BLEProtocol::AddressType_t>\r\n"
    "   * 'address'     : <the address which is a string representation like 'XX:XX:XX:XX:XX:XX'>\r\n",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
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

static constexpr const Command getMinAdvertisingInterval {
    "getMinAdvertisingInterval",
    "Return the minimum advertising interval",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getMinAdvertisingInterval());
    }
};

static constexpr const Command getMinNonConnectableAdvertisingInterval {
    "getMinNonConnectableAdvertisingInterval",
    // TODO DOC
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getMinNonConnectableAdvertisingInterval());
    }
};

static constexpr const Command getMaxAdvertisingInterval {
    "getMaxAdvertisingInterval",
    //TODO DOC
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getMaxAdvertisingInterval());
    }
};

static constexpr const Command stopAdvertising {
    "stopAdvertising",
    //TODO DOC
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().stopAdvertising();
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command stopScan {
    "stopScan",
    //TODO DOC
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().stopScan();
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command connect {
    "connect",
    "connect to a device, if this function succeed, a ConnectionCallbackParams_t is returned:\r\n"\
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
    "\t\tconnectionSupervisionTimeout: supervision timeout for this connection",
    (const CommandArgDescription[]) {
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
        { "<timeout>", "Maximum time allowed for this procedure" },
    },

    // TODO DOC
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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
        Gap::ConnectionParams_t connectionParams {
            minConnectionInterval,
            maxConnectionInterval,
            slaveLatency,
            connectionSupervisionTimeout
        };

        GapScanningParams scanParams {
            scanInterval,
            window,
            scanTimeout,
            activeScanning
        };

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
                auto& os = response->getResultStream();

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

        startProcedure<ConnectionProcedure>(addressType, address, response, procedureTimeout);
    }
};

static constexpr const Command disconnect {
    "disconnect",
    "disconnect the device from a specific connection.\r\n"\
    "If procedure succeed, a JSON object containing the following fields will be returned:\r\n"\
    "\t* handle: The handle disconnected\r\n"\
    "\t* reason: The reason of the disconnection (see Gap::DisconnectionReason_t)\r\n"\
    "In case of error, the reason of the error will be returned.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The id of the connection to terminate." },
        { "<reason>", "The reason of the termination (see Gap::DisconnectionReason_t)" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

        startProcedure<DisconnectionProcedure>(connectionHandle, reason, response, 3000);
    }
};

static constexpr const Command getPreferredConnectionParams {
    "getPreferredConnectionParams",
    // TODO DOC
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        Gap::ConnectionParams_t connectionParameters;
        ble_error_t err = gap().getPreferredConnectionParams(&connectionParameters);
        reportErrorOrSuccess(response, err, connectionParameters);
    }
};

static constexpr const Command setPreferredConnectionParams {
    "setPreferredConnectionParams",
    "set the prefered connection parameters",
    (const CommandArgDescription[]) {
        { "<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>", "all the parameters, coma separated" }
    },
    //TODO: betrer args
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

static constexpr const Command updateConnectionParams {
    "updateConnectionParams",
    // TODO DOC
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        // TODO
        //ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params)
        response->notImplemented();
    }
};

static constexpr const Command setDeviceName {
    "setDeviceName",
    "set the devce name",
    (const CommandArgDescription[]) {
        { "<name>", "the name of the device, it should not have space" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().setDeviceName((const uint8_t*) args[0]);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command getDeviceName {
    "getDeviceName",
    "return the device name as a string",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
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

static constexpr const  Command setAppearance {
    "setAppearance",
    "set the appearance flag of the device",
    (const CommandArgDescription[]) {
        { "<appearance>", "the appearance to set (see GapAdvertisingData::Appearance_t)" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        if(!fromString(args[0], appearance)) {
            response->invalidParameters("the appearance to set is illformed (see GapAdvertisingData::Appearance_t)");
            return;
        }

        ble_error_t err = gap().setAppearance(appearance);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command getAppearance {
    "getAppearance",
    "get the appearance of the device",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        ble_error_t err = gap().getAppearance(&appearance);
        reportErrorOrSuccess(response, err, appearance);
    }
};

static constexpr const Command setTxPower {
    "setTxPower",
    "set the transmission power of the device",
    (const CommandArgDescription[]) {
        { "<TxPower>", "The transmission power, it is an integer between [-128:127]"}
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        int8_t txPower = 0;
        if(!fromString(args[0], txPower)) {
            response->invalidParameters("the txPower is malformed (should be between [-127:128])");
            return;
        }

        ble_error_t err = gap().setTxPower(txPower);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command getPermittedTxPowerValues {
    "getPermittedTxPowerValues",
    "return an array of the authorized Tx power values",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        using namespace serialization;
        const int8_t* permittedTxPowerValues = NULL;
        size_t permittedTxPowerValuesCount = 0;

        gap().getPermittedTxPowerValues(&permittedTxPowerValues, &permittedTxPowerValuesCount);

        response->success();
        auto& os = response->getResultStream();
        os << startArray;
        std::for_each(permittedTxPowerValues, permittedTxPowerValues + permittedTxPowerValuesCount, [&](int8_t val) {
            os << val;
        });
        os << endArray;
    }
};

static constexpr const Command getState {
    "getState",
    "return the state of the device as defined in Gap::GapState_t",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getState());
    }
};

static constexpr const Command setAdvertisingType {
    "setAdvertisingType",
    "set the advertising type",
    (const CommandArgDescription[]) {
        { "<advertisingType>", "The advertising type as defined in GapAdvertisingParams::AdvertisingType_t" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        GapAdvertisingParams::AdvertisingType_t advType;

        if(!fromString(args[0], advType)) {
            response->invalidParameters("the advertising type is incorrect (see GapAdvertisingParams::AdvertisingType_t)");
            return;
        }

        gap().setAdvertisingType(advType);
        response->success();
    }
};

static constexpr const Command setAdvertisingInterval {
    "setAdvertisingInterval",
    "set the advertising interval",
    (const CommandArgDescription[]) {
        { "<interval>", "The interval in ms, if 0, the advertising is disabled" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            response->invalidParameters("the advertising interval is ill formed");
            return;
        }

        gap().setAdvertisingInterval(interval);
        response->success();
    }
};

static constexpr const Command setAdvertisingTimeout {
    "setAdvertisingTimeout",
    "set the advertising timeout, in seconds",
    (const CommandArgDescription[]) {
        { "<timeout>", "An integer wich represent the advertising timeout in seconds [0x1 : 0x3FFF]. 0 disable the timeout" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            response->invalidParameters("the advertising timeout is ill formed");
            return;
        }

        gap().setAdvertisingTimeout(timeout);
        response->success();
    }
};

static constexpr const Command startAdvertising {
    "startAdvertising",
    "start the advertising",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        ble_error_t err = gap().startAdvertising();
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command clearAdvertisingPayload {
    "clearAdvertisingPayload",
    "clear the advertising payload",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        gap().clearAdvertisingPayload();
        response->success();
    }
};

static constexpr const Command accumulateAdvertisingPayload {
    "accumulateAdvertisingPayload",
    "add a new field into the advertising payload",
    (const CommandArgDescription[]) {
        { "<fieldType>", "the field type of the data following (see GapAdvertisingData::DataType_t)" },
        { "<data>", "the value of the field, please see GapAdvertisingData" }
    },
    /* maximum args counts is undefined */ 0xFF,
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

static constexpr const Command updateAdvertisingPayload {
    "updateAdvertisingPayload",
    "update a field int the advertising payload.\r\n"
    "Take care, at the moment, this will only succeed if the new value has the same size as the old one",
    (const CommandArgDescription[]) {
        { "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
        { "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
    },
    /* maximum args counts is undefined */ 0xFF,
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

static constexpr const Command setAdvertisingPayload {
    "setAdvertisingPayload",
    "set the advertising payload",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        // TODO
        //ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
        response->notImplemented();
    }
};

static constexpr const Command getAdvertisingPayload {
    "getAdvertisingPayload",
    "get the current advertising payload",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getAdvertisingPayload());
    }
};

static constexpr const Command accumulateScanResponse {
    "accumulateScanResponse",
    "add a field into the scan response payload",
    (const CommandArgDescription[]) {
        { "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
        { "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
    },
    /* maximum args counts is undefined */ 0xFF,
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

static constexpr const Command clearScanResponse {
    "clearScanResponse",
    "clear the scan response",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        gap().clearScanResponse();
        response->success();
    }
};

static constexpr const Command setScanParams {
    "setScanParams",
    "Set the scan parameters",
    (const CommandArgDescription[]) {
        { "<interval>", "The scan interval, it should be a number between 3 and 10420ms" },
        { "<window>", "The scan window, it should be a number between 3 and 10420ms" },
        { "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " },
        { "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

static constexpr const Command setScanInterval {
    "setScanInterval",
    "set the scan interval",
    (const CommandArgDescription[]) {
        { "<interval>", "The interval between each scan, it should be a number between 3 and 10240ms" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            response->invalidParameters("the interval is ill formed");
            return;
        }

        ble_error_t err = gap().setScanInterval(interval);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command setScanWindow {
    "setScanWindow",
    "set the scan windows",
    (const CommandArgDescription[]) {
        { "<window>", "The interval between each scan, it should be a number between 3 and 10240ms" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t window = 0;
        if(!fromString(args[0], window)) {
            response->invalidParameters("the window is ill formed");
            return;
        }

        ble_error_t err = gap().setScanWindow(window);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command setScanTimeout {
    "setScanTimeout",
    "The scan timeout",
    (const CommandArgDescription[]) {
        { "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            response->invalidParameters("the timeout is ill formed");
            return;
        }

        ble_error_t err = gap().setScanTimeout(timeout);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command setActiveScanning {
    "setActiveScanning",
    "Enable or disable active scanning",
    (const CommandArgDescription[]) {
        { "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        bool activeScanning = 0;
        if(!fromString(args[0], activeScanning)) {
            response->invalidParameters("the active scanning state is ill formed");
            return;
        }

        ble_error_t err = gap().setActiveScanning(activeScanning);
        reportErrorOrSuccess(response, err);
    }
};

static constexpr const Command startScan {
    "startScan",
    "start the scan process",
    (const CommandArgDescription[]) {
        { "<duration>", "The duration of the scan" },
        { "<address>", "The address to scan for" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

        struct ScanProcedure : public AsyncProcedure {
            ScanProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, const Gap::Address_t& addr) :
                AsyncProcedure(res, timeout) {
                    memcpy(address, addr, sizeof(address));
            }

            virtual ~ScanProcedure() {
                // close the array of scan
                response->getResultStream() << serialization::endArray;
                gap().stopScan();
                // note : there should be a way to detach this function pointer in the BLE API
            }

            virtual bool doStart() {
                ble_error_t err = gap().startScan(this, &ScanProcedure::whenAdvertisementReceived);
                if(err) {
                    response->faillure(err);
                    return false;
                } else {
                    referenceTime = minar::ticks(minar::getTime());
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
                    key("time") << (minar::ticks(minar::getTime()) - referenceTime) <<
                endObject;
            }

            virtual void doWhenTimeout() {
                // nothing to do jere, timeout is not an error in this case
            }

            Gap::Address_t address;
            uint32_t referenceTime;
        };

        startProcedure<ScanProcedure>(response, duration, address);
    }
};

static constexpr const Command initRadioNotification {
    "initRadioNotification",
    "initialize radio notifications",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        // TODO (maybe)
        //ble_error_t initRadioNotification(void)
        response->notImplemented();
    }
};

static constexpr const Command getAdvertisingParams {
    "getAdvertisingParams",
    "return the current advertising params",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->success(gap().getAdvertisingParams());
    }
};

static constexpr const Command setAdvertisingParams {
    "setAdvertisingParams",
    "set the advertising parameters",
    (const CommandArgDescription[]) {
        { "<advertisingType>", "The Advertising type, please refer to GapAdvertisingParams::AdvertisingType_t" },
        { "<interval>", "The advertising interval, it should be a number between 0 and 65534" },
        { "<timeout>", "The timeout, it should be a number between 0 and 65534" },
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

} // end of annonymous namespace

ConstArray<Command> GapCommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        setAddress,
        getAddress,
        getMinAdvertisingInterval,
        getMinNonConnectableAdvertisingInterval,
        getMaxAdvertisingInterval,
        stopAdvertising,
        stopScan,
        connect,
        disconnect,
        getPreferredConnectionParams,
        setPreferredConnectionParams,
        updateConnectionParams,
        setDeviceName,
        getDeviceName,
        setAppearance,
        getAppearance,
        setTxPower,
        getPermittedTxPowerValues,
        getState,
        setAdvertisingType,
        setAdvertisingInterval,
        setAdvertisingTimeout,
        startAdvertising,
        clearAdvertisingPayload,
        accumulateAdvertisingPayload,
        updateAdvertisingPayload,
        setAdvertisingPayload,
        getAdvertisingPayload,
        accumulateScanResponse,
        clearScanResponse,
        setScanParams,
        setScanInterval,
        setScanWindow,
        setScanTimeout,
        setActiveScanning,
        startScan,
        initRadioNotification,
        getAdvertisingParams,
        setAdvertisingParams
    };

    return ConstArray<Command>(commandHandlers);
}
