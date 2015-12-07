#include "GattClientCommands.h"
#include "util/StaticLambda.h"
#include "ble/BLE.h"
#include "ble/DiscoveredService.h"
#include "ble/DiscoveredCharacteristic.h"
#include "minar/minar.h"
#include "CLICommand/CommandSuite.h"
#include "dynamic/Value.h"
#include "util/StaticString.h"
#include "util/DynamicString.h"

#include "Serialization/Serializer.h"
#include "Serialization/BleCommonSerializer.h"
#include "Serialization/UUID.h"
#include "Serialization/Hex.h"
#include "Serialization/DiscoveredCharacteristic.h"
#include "Serialization/GattCallbackParamTypes.h"


// TODO: description of returned results


// isolation
namespace {
static BLE& ble() {
    return BLE::Instance();
}

static GattClient& client() {
    return ble().gattClient();
}

static Gap& gap() {
    return ble().gap();
}

static constexpr const Command discoverAllServicesAndCharacteristics {
    "discoverAllServicesAndCharacteristics",
    "discover all services and characteristics available on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        static dynamic::Value result = nullptr;
        static void (*whenTerminated)(Gap::Handle_t) = nullptr;
        static void (*whenDisconnected)(const Gap::DisconnectionCallbackParams_t*) = nullptr;
        static minar::callback_handle_t timeoutHandle = nullptr;
        static auto returnResult = [] (CommandResult res) {
            CommandSuite<GattClientCommandSuiteDescription>::commandReady(
                discoverAllServicesAndCharacteristics.name,
                CommandArgs(0, 0), // command args are not saved right now, maybe later
                std::move(res)
            );
        };

        static auto detachRegisteredCallbacks = []() {
            client().onServiceDiscoveryTermination(nullptr);
            gap().onDisconnection().detach(whenDisconnected);
            if(timeoutHandle) {
                minar::Scheduler::cancelCallback(timeoutHandle);
                timeoutHandle = nullptr;
            }
        };

        static void (*serviceCallback)(const DiscoveredService*) = [](const DiscoveredService * discoveredService) {
            dynamic::Value service;

            service["UUID"_ss] = toString(discoveredService->getUUID());
            service["start_handle"_ss] = (int64_t) discoveredService->getStartHandle();
            service["end_handle"_ss] = (int64_t) discoveredService->getEndHandle();
            service["characteristics"_ss] = dynamic::Value::Array_t();

            result.push_back(std::move(service));
        };

        static void (*characteristicCallback)(const DiscoveredCharacteristic*) = [](const DiscoveredCharacteristic* discoveredCharacteristic) {
            // get the last service and push the characteristic in it
            auto lastService = result.array_end();
            --lastService;
            (*lastService)["characteristics"_ss].push_back(toDynamicValue(discoveredCharacteristic));
        };

        // get the connection handle
        static uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            return CommandResult::invalidParameters("the connection handle is ill formed"_ss);
        }

        // do the request
        ble_error_t err = client().launchServiceDiscovery(
            connectionHandle,
            serviceCallback,
            characteristicCallback
        );

        if(err) {
            return CommandResult::faillure(toString(err));
        }
        // prepare result as an array
        result = dynamic::Value::Array_t();

        // register for terminations callback

        // setup termination handle
        client().onServiceDiscoveryTermination(whenTerminated = [](Gap::Handle_t handle) {
            if(connectionHandle != handle) {
                return;
            };

            detachRegisteredCallbacks();
            returnResult(CommandResult::success(std::move(result)));
        });

        gap().onDisconnection(whenDisconnected = [](const Gap::DisconnectionCallbackParams_t* params) {
            if(connectionHandle != params->handle) {
                return;
            };

            detachRegisteredCallbacks();

            // construct the result for this error
            dynamic::Value discovered_services = std::move(result);
            result["discovered_services"_ss] = discovered_services;
            result["error"_ss] = "disconnection during discovery";

            returnResult(CommandResult::faillure(std::move(result)));
        });

        // register for timeout
        timeoutHandle = minar::Scheduler::postCallback([]() {
            timeoutHandle = nullptr;

            detachRegisteredCallbacks();

            // construct the result for this error
            dynamic::Value discovered_services = std::move(result);
            result["discovered_services"_ss] = discovered_services;
            result["error"_ss] = "discovery timeout";

            returnResult(CommandResult::faillure(std::move(result)));
        }).delay(minar::milliseconds(100 * 1000)).getHandle();


        return CommandResult(CMDLINE_RETCODE_EXCUTING_CONTINUE);
    }
};

static constexpr const Command discoverAllServices {
    "discoverAllServices",
    "discover all services available on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverPrimaryServicesByUUID {
    "discoverPrimaryServicesByUUID",
    "discover a specific kind of services on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceUUID>", "The UUID of the services to discover" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command findIncludedServices {
    "findIncludedServices",
    "Find includded services declaration withn a service definition on the server",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverCharacteristicsOfService {
    "discoverCharacteristicsOfService",
    "Discover all characteristics of a service, this procedure will find all the"
    "characteristics declaration within a service definition on a server",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverCharacteristicsByUUID {
    "discoverCharacteristicsByUUID",
    "Discover all characteristics of a service matching a specific UUID.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" },
        { "<serviceUUID>", "The UUID of the characteristics to discover" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverAllCharacteristicsDescriptors {
    "discoverAllCharacteristicsDescriptors",
    "Find all the characteristic descriptor’s Attribute Handles and Attribute "
    "Types within a characteristic definition. The characteristic specified is "
    "identified by the characteristic handle range.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle + 1 >",
          "The starting handle of the descriptors for this characteristic" },
        { "<endHandle>", "The ending handle of the characteristic definition" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};


// this class handle the read procedure
struct ReadProcedure {

    /**
     * @brief Launch the read procedure
     */
    static CommandResult launch(uint16_t _connectionHandle, uint16_t _valueHandle) {
        ble_error_t err = client().read(_connectionHandle, _valueHandle, /* offset */ 0);

        if (!err) {
            // no need to catch the result, it will automatially attach all callbacks,
            // and after a timeout or when the data has been read, it will release and
            // destroy itself automatically
            new ReadProcedure(_connectionHandle, _valueHandle);
            return CommandResult(CMDLINE_RETCODE_EXCUTING_CONTINUE);
        }

        return CommandResult::faillure(toString(err));
    }

private:

    /**
     * @brief Construct a read procedure, this will also attach all callbacks
     */
    ReadProcedure(uint16_t _connectionHandle, uint16_t _valueHandle) :
        connectionHandle(_connectionHandle), valueHandle(_valueHandle), timeoutHandle(nullptr) {
        // attach callbacks
        client().onDataRead(makeFunctionPointer(this, &ReadProcedure::whenDataRead));
        // attach timeout
        auto foo = mbed::util::FunctionPointer(this, &ReadProcedure::whenTimeout);

        timeoutHandle = minar::Scheduler::postCallback(this, &ReadProcedure::whenTimeout)
        .delay(minar::milliseconds(100 * 1000)).getHandle();
    }

    ~ReadProcedure() {
        // detach callbacks
        if (timeoutHandle) {
            minar::Scheduler::cancelCallback(timeoutHandle);
        }

        client().onDataRead().detach(makeFunctionPointer(this, &ReadProcedure::whenDataRead));
    }

    void whenDataRead(const GattReadCallbackParams* params) {
        // verifiy that it is the right characteristic on the right connection
        if (params->connHandle == connectionHandle && params->handle == valueHandle) {
            // convert value to hex
            returnResult(CommandResult::success(toDynamicValue(params)));

            delete this;
        }
    }

    void whenTimeout() {
        timeoutHandle = nullptr;
        returnResult(CommandResult::faillure("read procedure timeout"));
        delete this;
    }

    static void returnResult(CommandResult res) {
        CommandSuite<GattClientCommandSuiteDescription>::commandReady(
            "readCharacteristicValue",
            CommandArgs(0, 0), // command args are not saved right now, maybe later
            std::move(res)
        );
    };

private:
    uint16_t connectionHandle;
    uint16_t valueHandle;
    minar::callback_handle_t timeoutHandle;
};

static constexpr const Command readCharacteristicValue {
    "readCharacteristicValue",
    "Read a characteristic value from a GATT Server using a characteristic value handle.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "The handle of characteristic value" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            return CommandResult::invalidParameters("the connection handle is ill formed"_ss);
        }

        uint16_t characteristicValueHandle;
        if (!fromString(args[1], characteristicValueHandle)) {
            return CommandResult::invalidParameters("the characteristic value handle is ill formed"_ss);
        }

        return ReadProcedure::launch(connectionHandle, characteristicValueHandle);
    }
};

static constexpr const Command readUsingCharacteristicUUID {
    "readUsingCharacteristicUUID",
    "This sub-procedure is used to read a Characteristic Value from a server "
    "when the client only knows the characteristic UUID and does not know the "
    "handle of the characteristic.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" },
        { "<characteristicUUID>", "The UUID of the characteristic" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readLongCharacteristicValue {
    "readLongCharacteristicValue",
    "Read a characteristic value from a server when the client knows the "
    "characteristic value handle and the length of the characteristic value "
    "is longer than can be sent in a single read response",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "The handle of characteristic value" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readMultipleCharacteristicValues {
    "readMultipleCharacteristicValues",
    "Read a multiple characteristics values from a set of characteristics value "
    "handle.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandles...>", "Handles of characteristics values to read" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

class WriteProcedure {
public:
    static CommandResult launch(GattClient::WriteOp_t cmd, uint16_t _connectionHandle,
        uint16_t _valueHandle, container::Vector<uint8_t> dataToWrite) {
        ble_error_t err = client().write(
            cmd, _connectionHandle, _valueHandle, dataToWrite.size(), dataToWrite.begin()
        );

        if (!err) {
            // in this case, no response is expected from the server
            if(cmd == GattClient::GATT_OP_WRITE_CMD) {
                return CommandResult::success();
            } else {
                // no need to catch the result, it will automatially attach all callbacks,
                // and after a timeout or when the data has been writen, it will release and
                // destroy itself automatically
                new WriteProcedure(_connectionHandle, _valueHandle);
                return CommandResult(CMDLINE_RETCODE_EXCUTING_CONTINUE);
            }
        }

        return CommandResult::faillure(toString(err));
    }

private:

    /**
     * @brief Construct a read procedure, this will also attach all callbacks
     */
    WriteProcedure(uint16_t _connectionHandle, uint16_t _valueHandle) :
        connectionHandle(_connectionHandle), valueHandle(_valueHandle), timeoutHandle(nullptr) {
        // attach callbacks
        client().onDataWritten(makeFunctionPointer(this, &WriteProcedure::whenDataWritten));
        // attach timeout
        timeoutHandle = minar::Scheduler::postCallback(this, &WriteProcedure::whenTimeout)
        .delay(minar::milliseconds(100 * 1000)).getHandle();
    }

    ~WriteProcedure() {
        // detach callbacks
        if (timeoutHandle) {
            minar::Scheduler::cancelCallback(timeoutHandle);
        }

        client().onDataWritten().detach(makeFunctionPointer(this, &WriteProcedure::whenDataWritten));
    }

    void whenDataWritten(const GattWriteCallbackParams* params) {
        // verifiy that it is the right characteristic on the right connection
        if (params->connHandle == connectionHandle && params->handle == valueHandle) {
            // convert value to hex
            returnResult(CommandResult::success(toDynamicValue(params)));
            delete this;
        }
    }

    void whenTimeout() {
        timeoutHandle = nullptr;
        returnResult(CommandResult::faillure("write procedure timeout"));
        delete this;
    }

    static void returnResult(CommandResult res) {
        CommandSuite<GattClientCommandSuiteDescription>::commandReady(
            "writeXXX",
            CommandArgs(0, 0), // command args are not saved right now, maybe later
            std::move(res)
        );
    };

private:
    uint16_t connectionHandle;
    uint16_t valueHandle;
    minar::callback_handle_t timeoutHandle;
};

static constexpr const Command writeWithoutResponse {
    "writeWithoutResponse",
    "Write a characteristic value to a server, the server will not acknowledge anything.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            return CommandResult::faillure("connection handle is ill formed");
        }

        uint16_t characteristicValuehandle;
        if (!fromString(args[1], characteristicValuehandle)) {
            return CommandResult::faillure("characteristic value handle is ill formed");
        }

        auto dataToWrite = hexStringToRawData(args[2]);
        if(dataToWrite.size() == 0) {
            return CommandResult::faillure("data to write provided are invalids");
        }

        return WriteProcedure::launch(
            GattClient::GATT_OP_WRITE_CMD, connectionHandle, characteristicValuehandle, dataToWrite
        );
    }
};

static constexpr const Command signedWriteWithoutResponse {
    "signedWriteWithoutResponse",
    "Write a characteristic value to a server, the server will not acknowledge anything. "
    "This sub-procedure shall only be used if the CharacteristicProperties authenticated "
    "bit is enabled and the client and server device share a bond.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command write {
    "write",
    "Write a characteristic value to a server.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            return CommandResult::faillure("connection handle is ill formed");
        }

        uint16_t characteristicValuehandle;
        if (!fromString(args[1], characteristicValuehandle)) {
            return CommandResult::faillure("characteristic value handle is ill formed");
        }

        auto dataToWrite = hexStringToRawData(args[2]);
        if(dataToWrite.size() == 0) {
            return CommandResult::faillure("data to write provided are invalids");
        }

        return WriteProcedure::launch(
            GattClient::GATT_OP_WRITE_REQ, connectionHandle, characteristicValuehandle, dataToWrite
        );
    }
};

static constexpr const Command writeLong {
    "writeLong",
    "Write a characteristic value to a server. This sub-procedure is used when "
    "the client knows the Characteristic Value Handle but the length of the "
    "Characteristic Value is longer than can be sent in a single Write Request "
    "Attribute Protocol message.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command reliableWrite {
    "reliableWrite",
    "Write a characteristic value to a server. This sub-procedure is used when "
    "the client knows the Characteristic Value Handle, and assurance is required "
    "that the correct Characteristic Value is going to be written by transferring "
    "the Characteristic Value to be written in both directions before the write is "
    "performed. This sub-procedure can also be used when multiple values must be "
    "written, in order, in a single operation",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readCharacteristicDescriptor {
    "readCharacteristicDescriptor",
    "Read a characteristic descriptor from a server.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to read" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readLongCharacteristicDescriptor {
    "readLongCharacteristicDescriptor",
    "Read a characteristic descriptor from a server. This procedure is used "
    "when the length of the characteristic descriptor declaration is longer "
    "than what can be sent in a single read",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to read" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command writeCharacteristicDescriptor {
    "writeCharacteristicDescriptor",
    "Write a characteristic descriptor to a server.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command writeLongCharacteristicDescriptor {
    "writeLongCharacteristicDescriptor",
    "Write a characteristic descriptor to a server. This procedure when the "
    "client knows that the length of the characteristic descriptor value is "
    "longer than what can be sent in a single write.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

} // end of annonymous namespace

ConstArray<Command> GattClientCommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        discoverAllServicesAndCharacteristics,
        discoverAllServices,
        //discoverServices,
        findIncludedServices,
        discoverCharacteristicsOfService,
        discoverCharacteristicsByUUID,
        discoverAllCharacteristicsDescriptors,
        readCharacteristicValue,
        readUsingCharacteristicUUID,
        readLongCharacteristicValue,
        readMultipleCharacteristicValues,
        writeWithoutResponse,
        signedWriteWithoutResponse,
        write,
        writeLong,
        reliableWrite,
        readCharacteristicDescriptor,
        readLongCharacteristicDescriptor,
        writeCharacteristicDescriptor,
        writeLongCharacteristicDescriptor
    };

    return ConstArray<Command>(commandHandlers);
}
