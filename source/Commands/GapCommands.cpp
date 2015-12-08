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
#include "dynamic/Value.h"
#include "util/StaticString.h"
#include "util/DynamicString.h"

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

static constexpr const Command setAddress {
    "setAddress",
    "set the address of this device",
    (const CommandArgDescription[]) {
        { "<addressType>", "The address type to set. It is a string representation of Gap::AddressType_t" },
        { "<address>"    , "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\"" },
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        // extract first args
        Gap::AddressType_t addressType;
        if(!fromString(args[0], addressType)) {
            return CommandResult::invalidParameters("first argument should match Gap::AddressType_t"_ss);
        }

        Gap::Address_t address;
        if(!macAddressFromString(args[1], address)) {
            return CommandResult::invalidParameters("second argument should is a mac address which should match XX:XX:XX:XX:XX:XX format"_ss);
        }

        ble_error_t err = gap().setAddress(addressType, address);

        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command getAddress {
    "getAddress",
    "Get the address and the type of address of this device.\r\n"
    "The result will be a json object containing:\r\n"
    "   * 'address_type': <type of the address. It is a string representation of Gap::AddressType_t>\r\n"
    "   * 'address'     : <the address which is a string representation like 'XX:XX:XX:XX:XX:XX'>\r\n",
    STATIC_LAMBDA(const CommandArgs&) {
        Gap::AddressType_t addressType;
        Gap::Address_t address;

        ble_error_t err = gap().getAddress(&addressType, address);

        if(err) {
            return CommandResult::faillure(toString(err));
        }

        // building the result object
        dynamic::Value res;
        res["address_type"_ss] = container::StaticString(toString(addressType));
        res["address"_ss] = macAddressToString(address).str;

        return CommandResult::success(res);
    }
};

static constexpr const Command getMinAdvertisingInterval {
    "getMinAdvertisingInterval",
    "Return the minimum advertising interval",
    STATIC_LAMBDA(const CommandArgs&) {
        uint16_t minAdvertisingInterval = gap().getMinAdvertisingInterval();
        return CommandResult::success((int64_t) minAdvertisingInterval);
    }
};

static constexpr const Command getMinNonConnectableAdvertisingInterval {
    "getMinNonConnectableAdvertisingInterval",
    // TODO DOC
    STATIC_LAMBDA(const CommandArgs&) {
        uint16_t minNonConnectableAdvertisingInterval = gap().getMinNonConnectableAdvertisingInterval();
        return CommandResult::success((int64_t) minNonConnectableAdvertisingInterval);
    }
};

static constexpr const Command getMaxAdvertisingInterval {
    "getMaxAdvertisingInterval",
    //TODO DOC
    STATIC_LAMBDA(const CommandArgs&) {
        uint16_t maxAdvertisingInterval = gap().getMaxAdvertisingInterval();
        return CommandResult::success((int64_t) maxAdvertisingInterval);
    }
};

static constexpr const Command stopAdvertising {
    "stopAdvertising",
    //TODO DOC
    STATIC_LAMBDA(const CommandArgs&) {
        ble_error_t err = gap().stopAdvertising();
        return err ? CommandResult::faillure(container::StaticString(toString(err))) : CommandResult::success();
    }
};

static constexpr const Command stopScan {
    "stopScan",
    //TODO DOC
    STATIC_LAMBDA(const CommandArgs&) {
        ble_error_t err = gap().stopScan();
        return err ? CommandResult::faillure(container::StaticString(toString(err))) : CommandResult::success();
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
                           "It is a string representation of Gap::AddressType_t" },
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
    STATIC_LAMBDA(const CommandArgs& args) {
        // callback used when the device is connected
        static void (*whenConnected)(const Gap::ConnectionCallbackParams_t*) = nullptr;
        static minar::callback_handle_t timeoutHandle = nullptr;
        static bool connectionProcedureRunning = false;

        if (connectionProcedureRunning) {
           return CommandResult::faillure("a connection procedure is already running "_ss);
        }

        // extract Address and address type
        static Gap::AddressType_t addressType;
        if (!fromString(args[0], addressType)) {
            return CommandResult::invalidParameters("first argument should match Gap::AddressType_t"_ss);
        }

        static Gap::Address_t address;
        if (!macAddressFromString(args[1], address)) {
            return CommandResult::invalidParameters(
                "second argument should is a mac address which should match XX:XX:XX:XX:XX:XX format"_ss
            );
        }

        // extract connection parameters
        uint16_t minConnectionInterval;
        if (!fromString(args[2], minConnectionInterval)) {
            return CommandResult::invalidParameters("the minimum connection interval is ill formed"_ss);
        }

        uint16_t maxConnectionInterval;
        if (!fromString(args[3], maxConnectionInterval)) {
            return CommandResult::invalidParameters("the maximum connection interval is ill formed"_ss);
        }

        uint16_t slaveLatency;
        if (!fromString(args[4], slaveLatency)) {
            return CommandResult::invalidParameters("the slave latency is ill formed"_ss);
        }

        uint16_t connectionSupervisionTimeout;
        if (!fromString(args[5], connectionSupervisionTimeout)) {
            return CommandResult::invalidParameters("the connection supervision timeout is ill formed"_ss);
        }

        // extract scan parameters
        uint16_t scanInterval;
        if (!fromString(args[6], scanInterval)) {
            return CommandResult::invalidParameters("the scan interval is ill formed"_ss);
        }

        uint16_t window;
        if (!fromString(args[7], window)) {
            return CommandResult::invalidParameters("the scan window is ill formed"_ss);
        }

        uint16_t scanTimeout;
        if (!fromString(args[8], scanTimeout)) {
            return CommandResult::invalidParameters("the scan timeout is ill formed"_ss);
        }

        bool activeScanning;
        if (!fromString(args[9], activeScanning)) {
            return CommandResult::invalidParameters("the active scanning is ill formed"_ss);
        }

        // timeout for this procedure
        uint16_t procedureTimeout;
        if (!fromString(args[10], procedureTimeout)) {
            return CommandResult::invalidParameters("the procedure timeout is ill formed"_ss);
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
            return CommandResult::faillure(toString(err));
        }

        connectionProcedureRunning = true;

        // ok, the connection procedure has been launched, register callbacks and timeout
        whenConnected = [](const Gap::ConnectionCallbackParams_t* params) {
            // check that the callback has been called for the right address and object
            if (memcmp(params->peerAddr, address, sizeof(address)) != 0 ||
                params->peerAddrType != addressType) {
                return;
            }

            // ok, now it is possible to unregister this callback
            gap().onConnection().detach(whenConnected);

            // unregister timeout callback
            minar::Scheduler::cancelCallback(timeoutHandle);

            if (connectionProcedureRunning == false) {
                // nothing to do
                return;
            }

            connectionProcedureRunning = false;

            // constructing result
            dynamic::Value result;

            result["handle"_ss] = (int64_t) params->handle;
            result["role"_ss] = toString(params->role);
            result["peerAddrType"_ss] = toString(params->peerAddrType);
            result["peerAddr"_ss] = macAddressToString(params->peerAddr).str;
            result["ownAddrType"_ss] = toString(params->ownAddrType);
            result["ownAddr"_ss] = macAddressToString(params->ownAddr).str;
            result["connectionParams"_ss]["minConnectionInterval"_ss] = (int64_t) params->connectionParams->minConnectionInterval;
            result["connectionParams"_ss]["maxConnectionInterval"_ss] = (int64_t) params->connectionParams->maxConnectionInterval;
            result["connectionParams"_ss]["slaveLatency"_ss] = (int64_t) params->connectionParams->slaveLatency;
            result["connectionParams"_ss]["connectionSupervisionTimeout"_ss] = (int64_t) params->connectionParams->connectionSupervisionTimeout;

            CommandSuite<GapCommandSuiteDescription>::commandReady(
                connect.name,
                CommandArgs(0, 0), // command args are not saved right now, maybe later
                CommandResult::success(std::move(result))
            );
        };
        gap().onConnection(whenConnected);

        timeoutHandle = minar::Scheduler::postCallback([]() {
            // nothing to do
            if(connectionProcedureRunning == false) {
                return;
            }

            connectionProcedureRunning = false;

            // detach whenConnected handle
            gap().onConnection().detach(whenConnected);

            CommandSuite<GapCommandSuiteDescription>::commandReady(
                connect.name,
                CommandArgs(0, 0), // command args are not saved right now, maybe later
                CommandResult::faillure("timeout"_ss)
            );
        }).delay(minar::milliseconds(procedureTimeout)).getHandle();

        return CommandResult(CMDLINE_RETCODE_EXCUTING_CONTINUE);
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
    STATIC_LAMBDA(const CommandArgs& args) {
        static bool procedureRunning = false;
        static void (*whenDisconnected)(const Gap::DisconnectionCallbackParams_t*) = nullptr;

        // only one procedure can run
        if (procedureRunning) {
            return CommandResult::faillure("procedure already running");
        }

        static Gap::Handle_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            return CommandResult::invalidParameters("the connection handle is ill formed"_ss);
        }

        Gap::DisconnectionReason_t reason;
        if (!fromString(args[1], reason)) {
            return CommandResult::invalidParameters("the disconnection reason is ill formed"_ss);
        }

        // launch the procedure
        ble_error_t err = gap().disconnect(connectionHandle, reason);

        if (err) {
            return CommandResult::faillure(toString(err));
        }

        procedureRunning = true;

        // register the handler for disconnection
        whenDisconnected = [](const Gap::DisconnectionCallbackParams_t* params) {
            if (params->handle != connectionHandle) {
                return;
            }

            // unregister callbacks related to this procedure
            gap().onDisconnection().detach(whenDisconnected);

            if (!procedureRunning) {
                return;
            }

            procedureRunning = false;

            // the right connection has been disconnected, sending result back to the user
            dynamic::Value result;
            result["handle"_ss] = (int64_t) params->handle;
            result["reason"_ss] = toString(params->reason);

            CommandSuite<GapCommandSuiteDescription>::commandReady(
                disconnect.name,
                CommandArgs(0, 0), // command args are not saved right now, maybe later
                CommandResult::success(std::move(result))
            );
        };

        gap().onDisconnection(whenDisconnected);

        return CommandResult(CMDLINE_RETCODE_EXCUTING_CONTINUE);
    }
};

static constexpr const Command getPreferredConnectionParams {
    "getPreferredConnectionParams",
    // TODO DOC
    STATIC_LAMBDA(const CommandArgs&) {
        Gap::ConnectionParams_t connectionParameters;
        ble_error_t err = gap().getPreferredConnectionParams(&connectionParameters);

        if(err) {
            return CommandResult::faillure(container::StaticString(toString(err)));
        }
        return CommandResult::success(connectionParamsToJSON(connectionParameters));
    }
};

static constexpr const Command setPreferredConnectionParams {
    "setPreferredConnectionParams",
    "set the prefered connection parameters",
    (const CommandArgDescription[]) {
        { "<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>", "all the parameters, coma separated" }
    },
    //TODO: betrer args
    STATIC_LAMBDA(const CommandArgs& args) {
        Gap::ConnectionParams_t connectionParameters;
        if(!connectionParamsFromCLI(args[0], connectionParameters)) {
            return CommandResult::invalidParameters("malformed connection parameters, should be like"\
                "<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>"_ss);
        }

        ble_error_t err =  gap().setPreferredConnectionParams(&connectionParameters);
        return err ? CommandResult::faillure(container::StaticString(toString(err))) : CommandResult::success();
    }
};

static constexpr const Command updateConnectionParams {
    "updateConnectionParams",
    // TODO DOC
    STATIC_LAMBDA(const CommandArgs&) {
        // TODO
        //ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params)
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command setDeviceName {
    "setDeviceName",
    "set the devce name",
    (const CommandArgDescription[]) {
        { "<name>", "the name of the device, it should not have space" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        ble_error_t err = gap().setDeviceName((const uint8_t*) args[0]);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command getDeviceName {
    "getDeviceName",
    "return the device name as a string",
    STATIC_LAMBDA(const CommandArgs&) {
        // first : collect the size of the name
        unsigned deviceNameLength;
        ble_error_t err = gap().getDeviceName(NULL, &deviceNameLength);

        if(err) {
            return CommandResult::faillure("impossible to get the lenght of the name");
        }

        // acquire the name
        ++deviceNameLength;  // add null terminator to the length
        uint8_t* deviceName = static_cast<uint8_t*>(malloc(deviceNameLength));
        memset(deviceName, 0, deviceNameLength);
        err = gap().getDeviceName(deviceName, &deviceNameLength);

        if(err) {
            free(deviceName);
            return CommandResult::faillure(toString(err));
        }

        CommandResult res = CommandResult::success((const char*) deviceName);
        free(deviceName);
        return res;
    }
};

static constexpr const  Command setAppearance {
    "setAppearance",
    "set the appearance flag of the device",
    (const CommandArgDescription[]) {
        { "<appearance>", "the appearance to set (see GapAdvertisingData::Appearance_t)" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        if(!fromString(args[0], appearance)) {
            return CommandResult::invalidParameters("the appearance to set is illformed (see GapAdvertisingData::Appearance_t)");
        }

        ble_error_t err = gap().setAppearance(appearance);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command getAppearance {
    "getAppearance",
    "get the appearance of the device",
    STATIC_LAMBDA(const CommandArgs&) {
        GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
        ble_error_t err = gap().getAppearance(&appearance);

        if(err) {
            return CommandResult::faillure(container::StaticString(toString(err)));
        }

        return CommandResult::success(toString(appearance));
    }
};

static constexpr const Command setTxPower {
    "setTxPower",
    "set the transmission power of the device",
    (const CommandArgDescription[]) {
        { "<TxPower>", "The transmission power, it is an integer between [-128:127]"}
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        int8_t txPower = 0;
        if(!fromString(args[0], txPower)) {
            return CommandResult::invalidParameters("the txPower is malformed (should be between [-127:128])");
        }

        ble_error_t err = gap().setTxPower(txPower);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command getPermittedTxPowerValues {
    "getPermittedTxPowerValues",
    "return an array of the authorized Tx power values",
    STATIC_LAMBDA(const CommandArgs&) {
        const int8_t* permittedTxPowerValues = NULL;
        size_t permittedTxPowerValuesCount = 0;

        gap().getPermittedTxPowerValues(&permittedTxPowerValues, &permittedTxPowerValuesCount);

        return CommandResult::success(txPermittedValuesToJSON(permittedTxPowerValues, permittedTxPowerValuesCount));
    }
};

static constexpr const Command getState {
    "getState",
    "return the state of the device as defined in Gap::GapState_t",
    STATIC_LAMBDA(const CommandArgs&) {
        Gap::GapState_t state = gap().getState();
        return CommandResult::success(gapStateToJSON(state));
    }
};

static constexpr const Command setAdvertisingType {
    "setAdvertisingType",
    "set the advertising type",
    (const CommandArgDescription[]) {
        { "<advertisingType>", "The advertising type as defined in GapAdvertisingParams::AdvertisingType_t" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        GapAdvertisingParams::AdvertisingType_t advType;

        if(!fromString(args[0], advType)) {
            return CommandResult::invalidParameters("the advertising type is incorrect (see GapAdvertisingParams::AdvertisingType_t)");
        }

        gap().setAdvertisingType(advType);

        return CommandResult::success();
    }
};

static constexpr const Command setAdvertisingInterval {
    "setAdvertisingInterval",
    "set the advertising interval",
    (const CommandArgDescription[]) {
        { "<interval>", "The interval in ms, if 0, the advertising is disabled" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            return CommandResult::invalidParameters("the advertising interval is ill formed");
        }

        gap().setAdvertisingInterval(interval);

        return CommandResult::success();
    }
};

static constexpr const Command setAdvertisingTimeout {
    "setAdvertisingTimeout",
    "set the advertising timeout, in seconds",
    (const CommandArgDescription[]) {
        { "<timeout>", "An integer wich represent the advertising timeout in seconds [0x1 : 0x3FFF]. 0 disable the timeout" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            return CommandResult::invalidParameters("the advertising timeout is ill formed");
        }

        gap().setAdvertisingTimeout(timeout);

        return CommandResult::success();
    }
};

static constexpr const Command startAdvertising {
    "startAdvertising",
    "start the advertising",
    STATIC_LAMBDA(const CommandArgs&) {
        ble_error_t err = gap().startAdvertising();
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command clearAdvertisingPayload {
    "clearAdvertisingPayload",
    "clear the advertising payload",
    STATIC_LAMBDA(const CommandArgs&) {
        gap().clearAdvertisingPayload();
        return CommandResult::success();
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
    STATIC_LAMBDA(const CommandArgs& args) {
        AdvertisingPayloadField_t payloadField;
        const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

        if(parsingError) {
            return CommandResult::invalidParameters(parsingError);
        }

        ble_error_t err = gap().accumulateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
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
    STATIC_LAMBDA(const CommandArgs& args) {
        AdvertisingPayloadField_t payloadField;
        const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

        if(parsingError) {
            return CommandResult::invalidParameters(parsingError);
        }

        ble_error_t err = gap().updateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command setAdvertisingPayload {
    "setAdvertisingPayload",
    "set the advertising payload",
    STATIC_LAMBDA(const CommandArgs&) {
        // TODO
        //ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command getAdvertisingPayload {
    "getAdvertisingPayload",
    "get the current advertising payload",
    STATIC_LAMBDA(const CommandArgs&) {
        const GapAdvertisingData& advertisingData = gap().getAdvertisingPayload();
        return CommandResult::success(gapAdvertisingDataToJSON(advertisingData));
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
    STATIC_LAMBDA(const CommandArgs& args) {
        AdvertisingPayloadField_t payloadField;
        const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

        if(parsingError) {
            return CommandResult::invalidParameters(parsingError);
        }

        ble_error_t err = gap().accumulateScanResponse(payloadField.dataType, payloadField.data, payloadField.dataLenght);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command clearScanResponse {
    "clearScanResponse",
    "clear the scan response",
    STATIC_LAMBDA(const CommandArgs&) {
        gap().clearScanResponse();
        return CommandResult::success();
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
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t interval = 0xFFFF;
        if(!fromString(args[0], interval)) {
            return CommandResult::invalidParameters("invalid interval, it should be a number between 3 and 10240ms");
        }

        uint16_t window = 0xFFFF;
        if(!fromString(args[1], window)) {
            return CommandResult::invalidParameters("invalid window, it should be a number between 3 and 10240ms");
        }

        uint16_t timeout = 0;
        if(!fromString(args[2], timeout)) {
            return CommandResult::invalidParameters("invalid timeout, it should be a number between 0 and 65534");
        }

        uint16_t activeScanning = 0;
        if(!fromString(args[3], activeScanning)) {
            return CommandResult::invalidParameters("invalid activeScaning, it should be a number boolean value");
        }

        ble_error_t err = gap().setScanParams(interval, window, timeout, activeScanning);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command setScanInterval {
    "setScanInterval",
    "set the scan interval",
    (const CommandArgDescription[]) {
        { "<interval>", "The interval between each scan, it should be a number between 3 and 10240ms" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t interval = 0;
        if(!fromString(args[0], interval)) {
            return CommandResult::invalidParameters("the interval is ill formed");
        }

        ble_error_t err = gap().setScanInterval(interval);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command setScanWindow {
    "setScanWindow",
    "set the scan windows",
    (const CommandArgDescription[]) {
        { "<window>", "The interval between each scan, it should be a number between 3 and 10240ms" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t window = 0;
        if(!fromString(args[0], window)) {
            return CommandResult::invalidParameters("the window is ill formed");
        }

        ble_error_t err = gap().setScanWindow(window);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command setScanTimeout {
    "setScanTimeout",
    "The scan timeout",
    (const CommandArgDescription[]) {
        { "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t timeout = 0;
        if(!fromString(args[0], timeout)) {
            return CommandResult::invalidParameters("the timeout is ill formed"_ss);
        }

        ble_error_t err = gap().setScanTimeout(timeout);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command setActiveScanning {
    "setActiveScanning",
    "Enable or disable active scanning",
    (const CommandArgDescription[]) {
        { "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        bool activeScanning = 0;
        if(!fromString(args[0], activeScanning)) {
            return CommandResult::invalidParameters("the active scanning state is ill formed"_ss);
        }

        ble_error_t err = gap().setActiveScanning(activeScanning);
        return err ? CommandResult::faillure(toString(err)) : CommandResult::success();
    }
};

static constexpr const Command startScan {
    "startScan",
    "start the scan process",
    (const CommandArgDescription[]) {
        { "<duration>", "The duration of the scan" },
        { "<address>", "The address to scan for" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        static bool callAttached = false;
        static dynamic::Value results;
        static bool scanning = false;
        static Gap::Address_t address;
        static uint32_t referenceTime;

        if(scanning == true) {
            return CommandResult::faillure("a scan is already running"_ss);
        }

        uint16_t duration = 0;
        if(!fromString(args[0], duration)) {
            return CommandResult::invalidParameters("first argument should be the duration of the scan in milliseconds"_ss);
        }

        if(!macAddressFromString(args[1], address)) {
            return CommandResult::invalidParameters("second argument should be a mac address which should match XX:XX:XX:XX:XX:XX format"_ss);
        }

        // clear the previous results
        results = nullptr;

        ble_error_t err;
        if(/* callAttached == false */ true) {
            err = gap().startScan([] (const Gap::AdvertisementCallbackParams_t* scanResult) {
                if(scanning == false || memcmp(scanResult->peerAddr, address, sizeof(address))) {
                    return;
                }

                // setup reference time if there is no previous record
                if(results == nullptr) {
                    referenceTime = minar::ticks(minar::getTime());
                }

                dynamic::Value result;
                result["peerAddr"_ss] = macAddressToString(scanResult->peerAddr).str;
                result["rssi"_ss] = (int64_t) scanResult->rssi;
                result["isScanResponse"_ss] = scanResult->isScanResponse;
                result["type"_ss] = container::StaticString(toString(scanResult->type));
                result["data"_ss] = gapAdvertisingDataToJSON(scanResult->advertisingData, scanResult->advertisingDataLen);
                result["time"_ss] = (int64_t) (minar::ticks(minar::getTime()) - referenceTime);

                results.push_back(result);
            });
            callAttached = true;
        } else {
            err = gap().startScan(NULL);
        }

        if(err) {
            return CommandResult::faillure(toString(err));
        }

        scanning = true;

        // stop the scan after the timeout
        minar::Scheduler::postCallback([]() {
            // check for error
            gap().stopScan();
            scanning = false;
            CommandSuite<GapCommandSuiteDescription>::commandReady(startScan.name, CommandArgs(0, 0), CommandResult::success(std::move(results)));
            results = nullptr;
        }).delay(minar::milliseconds(duration));

        return CommandResult(CMDLINE_RETCODE_EXCUTING_CONTINUE);
    }
};

static constexpr const Command initRadioNotification {
    "initRadioNotification",
    "initialize radio notifications",
    STATIC_LAMBDA(const CommandArgs&) {
        // TODO (maybe)
        //ble_error_t initRadioNotification(void)
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command getAdvertisingParams {
    "getAdvertisingParams",
    "return the current advertising params",
    STATIC_LAMBDA(const CommandArgs&) {
        GapAdvertisingParams& advertisingParams = gap().getAdvertisingParams();
        return CommandResult::success(advertisingParamsToJSON(advertisingParams));
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
    STATIC_LAMBDA(const CommandArgs& args) {
        GapAdvertisingParams::AdvertisingType_t advertisingType;
        if(!fromString(args[0], advertisingType)) {
            return CommandResult::invalidParameters("Advertising type is malformed, please refer to GapAdvertisingParams::AdvertisingType_t"_ss);
        }

        uint16_t interval;
        if(!fromString(args[1], interval)) {
            return CommandResult::invalidParameters("Advertising interval is malformed, should be a number between 0 and 65534"_ss);
        }

        uint16_t timeout;
        if(!fromString(args[2], timeout)) {
            return CommandResult::invalidParameters("Advertising timeout is malformed, should be a number between 0 and 65534"_ss);
        }

        gap().setAdvertisingParams(GapAdvertisingParams(advertisingType, interval, timeout));
        return CommandResult::success();
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
