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
#include "Serialization/BLECommonSerializer.h"
#include "Serialization/UUID.h"
#include "Serialization/Hex.h"
#include "Serialization/DiscoveredCharacteristic.h"
#include "Serialization/GattCallbackParamTypes.h"
#include "util/AsyncProcedure.h"




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
    STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
        // get the connection handle
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        struct DiscoverAllServicesAndCharacteristicsProcedure : public AsyncProcedure {
            DiscoverAllServicesAndCharacteristicsProcedure(const std::shared_ptr<CommandResponse>& res, uint32_t timeout, uint16_t handle) :
                AsyncProcedure(res, timeout), connectionHandle(handle), isFirstServiceDiscovered(true) {
            }

            ~DiscoverAllServicesAndCharacteristicsProcedure() {
                client().onServiceDiscoveryTermination(nullptr);
                gap().onDisconnection().detach(makeFunctionPointer(
                    this, &DiscoverAllServicesAndCharacteristicsProcedure::whenDisconnected
                ));
            }

            virtual bool doStart() {
                ble_error_t err = client().launchServiceDiscovery(
                    connectionHandle,
                    makeFunctionPointer(this, &DiscoverAllServicesAndCharacteristicsProcedure::whenServiceDiscovered),
                    makeFunctionPointer(this, &DiscoverAllServicesAndCharacteristicsProcedure::whenCharacteristicDiscovered)
                );

                if(err) {
                    response->faillure(err);
                    return false;
                }

                client().onServiceDiscoveryTermination(makeFunctionPointer(
                    this, &DiscoverAllServicesAndCharacteristicsProcedure::whenServiceDiscoveryTerminated
                ));

                gap().onDisconnection(makeFunctionPointer(
                    this, &DiscoverAllServicesAndCharacteristicsProcedure::whenDisconnected
                ));

                response->getResultStream() << serialization::startArray;
                return true;
            }

            void whenServiceDiscovered(const DiscoveredService * discoveredService) {
                using namespace serialization;

                if(isFirstServiceDiscovered) {
                    isFirstServiceDiscovered = false;
                } else {
                    response->getResultStream() << endArray << endObject;
                }

                response->getResultStream() <<  startObject <<
                    key("UUID") << discoveredService->getUUID() <<
                    key("start_handle") << discoveredService->getStartHandle() <<
                    key("end_handle") << discoveredService->getEndHandle() <<
                    key("characteristics") << startArray;
            }

            void whenCharacteristicDiscovered(const DiscoveredCharacteristic* discoveredCharacteristic) {
                response->getResultStream() << *discoveredCharacteristic;
            }

            void whenServiceDiscoveryTerminated(Gap::Handle_t handle) {
                using namespace serialization;

                if(connectionHandle != handle) {
                    return;
                };

                if(isFirstServiceDiscovered == false) {
                    response->getResultStream() << endArray << endObject;
                }

                response->getResultStream() << endArray;

                response->success();
                terminate();
            }

            void whenDisconnected(const Gap::DisconnectionCallbackParams_t* params) {
                using namespace serialization;

                if(connectionHandle != params->handle) {
                    return;
                };

                if(isFirstServiceDiscovered == false) {
                    response->getResultStream() << endArray << endObject;
                }

                response->getResultStream() << "disconnection during discovery";
                response->faillure();
            }

            virtual void doWhenTimeout() {
                using namespace serialization;

                if(isFirstServiceDiscovered == false) {
                    response->getResultStream() << endArray << endObject;
                }

                response->getResultStream() << "discovery timeout";
                response->faillure();
            }

            uint16_t connectionHandle;
            bool isFirstServiceDiscovered;
        };


        startProcedure<DiscoverAllServicesAndCharacteristicsProcedure>(
            response, /* timeout */ 100 * 1000, connectionHandle
        );

    }
};

static constexpr const Command discoverAllServices {
    "discoverAllServices",
    "discover all services available on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" }
    },
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
    }
};

static constexpr const Command discoverPrimaryServicesByUUID {
    "discoverPrimaryServicesByUUID",
    "discover a specific kind of services on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceUUID>", "The UUID of the services to discover" }
    },
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
    }
};


// this class handle the read procedure
struct ReadProcedure : public AsyncProcedure {
    /**
     * @brief Construct a read procedure, this will also attach all callbacks
     */
    ReadProcedure(const std::shared_ptr<CommandResponse>& res, uint32_t timeout, uint16_t _connectionHandle, uint16_t _valueHandle) :
        AsyncProcedure(res, timeout), connectionHandle(_connectionHandle), valueHandle(_valueHandle) {
    }

    ~ReadProcedure() {
        // detach callbacks
        client().onDataRead().detach(makeFunctionPointer(this, &ReadProcedure::whenDataRead));
    }

    bool doStart() {
        ble_error_t err = client().read(connectionHandle, valueHandle, /* offset */ 0);
        if (err) {
            response->faillure(err);
            return false;
        }

        // attach callbacks
        client().onDataRead(makeFunctionPointer(this, &ReadProcedure::whenDataRead));
        return true;
    }

    void whenDataRead(const GattReadCallbackParams* params) {
        // verifiy that it is the right characteristic on the right connection
        if (params->connHandle == connectionHandle && params->handle == valueHandle) {
            // convert value to hex
            response->success(*params);
            terminate();
        }
    }

private:
    uint16_t connectionHandle;
    uint16_t valueHandle;
};

static constexpr const Command readCharacteristicValue {
    "readCharacteristicValue",
    "Read a characteristic value from a GATT Server using a characteristic value handle.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "The handle of characteristic value" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {

        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        uint16_t characteristicValueHandle;
        if (!fromString(args[1], characteristicValueHandle)) {
            response->invalidParameters("the characteristic value handle is ill formed");
            return;
        }

        startProcedure<ReadProcedure>(response, /* timeout */ 100 * 1000, connectionHandle, characteristicValueHandle);
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
    }
};

struct WriteProcedure : public AsyncProcedure {
    WriteProcedure(const std::shared_ptr<CommandResponse>& res, uint32_t timeout,
        GattClient::WriteOp_t _cmd, uint16_t _connectionHandle, uint16_t _valueHandle, container::Vector<uint8_t> _dataToWrite) :
        AsyncProcedure(res, timeout), cmd(_cmd), connectionHandle(_connectionHandle),
        valueHandle(_valueHandle), dataToWrite(_dataToWrite) {
    }

    ~WriteProcedure() {
        client().onDataWritten().detach(makeFunctionPointer(this, &WriteProcedure::whenDataWritten));
    }

    bool doStart() {
        ble_error_t err = client().write(
            cmd, connectionHandle, valueHandle, dataToWrite.size(), dataToWrite.begin()
        );

        if(err) {
            response->faillure(toString(err));
            return false;
        }

        // in this case, no response is expected from the server
        if(cmd == GattClient::GATT_OP_WRITE_CMD) {
            response->success();
            // terminate here
            return false;
        }

        // attach callbacks
        client().onDataWritten(makeFunctionPointer(this, &WriteProcedure::whenDataWritten));
        return true;
    }

    void whenDataWritten(const GattWriteCallbackParams* params) {
        // verifiy that it is the right characteristic on the right connection
        if (params->connHandle == connectionHandle && params->handle == valueHandle) {
            // convert value to hex
            response->success(*params);
            terminate();
        }
    }

private:
    GattClient::WriteOp_t cmd;
    uint16_t connectionHandle;
    uint16_t valueHandle;
    container::Vector<uint8_t> dataToWrite;
};


static constexpr const Command writeWithoutResponse {
    "writeWithoutResponse",
    "Write a characteristic value to a server, the server will not acknowledge anything.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->faillure("connection handle is ill formed");
            return;
        }

        uint16_t characteristicValuehandle;
        if (!fromString(args[1], characteristicValuehandle)) {
            response->faillure("characteristic value handle is ill formed");
            return;
        }

        auto dataToWrite = hexStringToRawData(args[2]);
        if(dataToWrite.size() == 0) {
            response->faillure("data to write provided are invalids");
            return;
        }

        startProcedure<WriteProcedure>(
            response, /* timeout */ 100 * 1000,
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->faillure("connection handle is ill formed");
            return;
        }

        uint16_t characteristicValuehandle;
        if (!fromString(args[1], characteristicValuehandle)) {
            response->faillure("characteristic value handle is ill formed");
            return;
        }

        auto dataToWrite = hexStringToRawData(args[2]);
        if(dataToWrite.size() == 0) {
            response->faillure("data to write provided are invalids");
            return;
        }

        startProcedure<WriteProcedure>(
            response, 100 * 1000,
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
    }
};

static constexpr const Command readCharacteristicDescriptor {
    "readCharacteristicDescriptor",
    "Read a characteristic descriptor from a server.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to read" }
    },
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
    STATIC_LAMBDA(const CommandArgs&, const std::shared_ptr<CommandResponse>& response) {
        response->notImplemented();
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
