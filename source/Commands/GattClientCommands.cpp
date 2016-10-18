#include "GattClientCommands.h"
#include "ble/BLE.h"
#include "ble/DiscoveredService.h"
#include "ble/DiscoveredCharacteristic.h"
#include "CLICommand/CommandSuite.h"

#include "Serialization/Serializer.h"
#include "Serialization/BLECommonSerializer.h"
#include "Serialization/UUID.h"
#include "Serialization/Hex.h"
#include "Serialization/DiscoveredCharacteristic.h"
#include "Serialization/GattCallbackParamTypes.h"
#include "util/AsyncProcedure.h"

using mbed::util::SharedPointer;

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


struct DiscoverAllServicesAndCharacteristicsCommand : public BaseCommand {
    static const char* name() {
        return "discoverAllServicesAndCharacteristics";
    }

    static const char* help() {
        return "discover all services and characteristics available on a peer device";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        // get the connection handle
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        startProcedure<DiscoverAllServicesAndCharacteristicsProcedure>(
            response, /* timeout */ 30 * 1000, connectionHandle
        );
    }

    struct DiscoverAllServicesAndCharacteristicsProcedure : public AsyncProcedure {
        DiscoverAllServicesAndCharacteristicsProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, uint16_t handle) :
            AsyncProcedure(res, timeout), connectionHandle(handle), isFirstServiceDiscovered(true) {
        }

        virtual ~DiscoverAllServicesAndCharacteristicsProcedure() {
            client().onServiceDiscoveryTermination(NULL);
            client().terminateServiceDiscovery();
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

            terminate();
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
};


struct DiscoverAllServicesCommand : public BaseCommand {
    static const char* name() {
        return "discoverAllServices";
    }

    static const char* help() {
        return "discover all services available on a peer device";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        // get the connection handle
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        startProcedure<DiscoverAllServicesProcedure>(
            response, /* timeout */ 30 * 1000, connectionHandle
        );
    }

    struct DiscoverAllServicesProcedure : public AsyncProcedure {
        DiscoverAllServicesProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, uint16_t handle) :
            AsyncProcedure(res, timeout), connectionHandle(handle) {
        }

        virtual ~DiscoverAllServicesProcedure() {
            client().onServiceDiscoveryTermination(NULL);
            gap().onDisconnection().detach(makeFunctionPointer(
                this, &DiscoverAllServicesProcedure::whenDisconnected
            ));
        }

        virtual bool doStart() {
            ble_error_t err = client().discoverServices(
                connectionHandle,
                makeFunctionPointer(this, &DiscoverAllServicesProcedure::whenServiceDiscovered)
            );

            if(err) {
                response->faillure(err);
                return false;
            }

            client().onServiceDiscoveryTermination(makeFunctionPointer(
                this, &DiscoverAllServicesProcedure::whenServiceDiscoveryTerminated
            ));

            gap().onDisconnection(makeFunctionPointer(
                this, &DiscoverAllServicesProcedure::whenDisconnected
            ));

            response->getResultStream() << serialization::startArray;
            return true;
        }

        void whenServiceDiscovered(const DiscoveredService * discoveredService) {
            using namespace serialization;

            response->getResultStream() <<  startObject <<
                key("UUID") << discoveredService->getUUID() <<
                key("start_handle") << discoveredService->getStartHandle() <<
                key("end_handle") << discoveredService->getEndHandle() <<
            endObject;
        }

        void whenServiceDiscoveryTerminated(Gap::Handle_t handle) {
            using namespace serialization;

            if(connectionHandle != handle) {
                return;
            };

            response->getResultStream() << endArray;
            response->success();
            terminate();
        }

        void whenDisconnected(const Gap::DisconnectionCallbackParams_t* params) {
            if(connectionHandle != params->handle) {
                return;
            };

            response->getResultStream() << "disconnection during discovery";
            response->faillure();
            terminate();
        }

        virtual void doWhenTimeout() {
            response->getResultStream() << "discovery timeout";
            response->faillure();
        }

        uint16_t connectionHandle;
    };
};


struct DiscoverPrimaryServicesByUUIDCommand : public BaseCommand {
    static const char* name() {
        return "discoverPrimaryServicesByUUID";
    }

    static const char* help() {
        return "discover a specific kind of services on a peer device";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<serviceUUID>", "The UUID of the services to discover" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        // get the connection handle
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        UUID serviceUUID;
        if (!fromString(args[1], serviceUUID)) {
            response->invalidParameters("the UUID is ill formed");
            return;
        }

        startProcedure<DiscoverServicesByUUIDProcedure>(
            response, /* timeout */ 30 * 1000, connectionHandle, serviceUUID
        );
    }

    struct DiscoverServicesByUUIDProcedure : public AsyncProcedure {
        DiscoverServicesByUUIDProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, uint16_t handle, const UUID& uuid) :
            AsyncProcedure(res, timeout), connectionHandle(handle), serviceUUID(uuid) {
        }

        virtual ~DiscoverServicesByUUIDProcedure() {
            client().onServiceDiscoveryTermination(NULL);
            gap().onDisconnection().detach(makeFunctionPointer(
                this, &DiscoverServicesByUUIDProcedure::whenDisconnected
            ));
        }

        virtual bool doStart() {
            ble_error_t err = client().discoverServices(
                connectionHandle,
                makeFunctionPointer(this, &DiscoverServicesByUUIDProcedure::whenServiceDiscovered),
                serviceUUID
            );

            if(err) {
                response->faillure(err);
                return false;
            }

            client().onServiceDiscoveryTermination(makeFunctionPointer(
                this, &DiscoverServicesByUUIDProcedure::whenServiceDiscoveryTerminated
            ));

            gap().onDisconnection(makeFunctionPointer(
                this, &DiscoverServicesByUUIDProcedure::whenDisconnected
            ));

            response->getResultStream() << serialization::startArray;
            return true;
        }

        void whenServiceDiscovered(const DiscoveredService * discoveredService) {
            using namespace serialization;

            response->getResultStream() <<  startObject <<
                key("UUID") << discoveredService->getUUID() <<
                key("start_handle") << discoveredService->getStartHandle() <<
                key("end_handle") << discoveredService->getEndHandle() <<
            endObject;
        }

        void whenServiceDiscoveryTerminated(Gap::Handle_t handle) {
            using namespace serialization;

            if(connectionHandle != handle) {
                return;
            };

            response->getResultStream() << endArray;
            response->success();
            terminate();
        }

        void whenDisconnected(const Gap::DisconnectionCallbackParams_t* params) {
            if(connectionHandle != params->handle) {
                return;
            };

            response->getResultStream() << "disconnection during discovery";
            response->faillure();
            terminate();
        }

        virtual void doWhenTimeout() {
            response->getResultStream() << "discovery timeout";
            response->faillure();
        }

        uint16_t connectionHandle;
        UUID serviceUUID;
    };
};


struct FindIncludedServicesCommand : public BaseCommand {
    static const char* name() {
        return "findIncludedServices";
    }

    static const char* help() {
        return "Find includded services declaration withn a service definition on the server";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<serviceStartHandle>", "The starting handle of the service" },
            { "<serviceEndHandle>", "The ending handle of the service" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};



struct DiscoverCharacteristicsOfServiceCommand : public BaseCommand {
    static const char* name() {
        return "discoverCharacteristicsOfService";
    }

    static const char* help() {
        return "Discover all characteristics of a service, this procedure will find all the"
               "characteristics declaration within a service definition on a server";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<serviceStartHandle>", "The starting handle of the service" },
            { "<serviceEndHandle>", "The ending handle of the service" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct DiscoverCharacteristicsByUUIDCommand : public BaseCommand {
    static const char* name() {
        return "discoverCharacteristicsByUUID";
    }

    static const char* help() {
        return "Discover all characteristics of a service matching a specific UUID.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<serviceStartHandle>", "The starting handle of the service" },
            { "<serviceEndHandle>", "The ending handle of the service" },
            { "<serviceUUID>", "The UUID of the characteristics to discover" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct DiscoverAllCharacteristicsDescriptorsCommand : public BaseCommand {
    static const char* name() {
        return "discoverAllCharacteristicsDescriptors";
    }

    static const char* help() {
        return "Find all the characteristic descriptor’s Attribute Handles and Attribute "
               "Types within a characteristic definition. The characteristic specified is "
               "identified by the characteristic handle range.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicStartHandle>",
              "The start handle of of the characteristic" },
            { "<endHandle>", "The ending handle of the characteristic definition" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        uint16_t startHandle;
        if (!fromString(args[1], startHandle)) {
            response->invalidParameters("The value handle is ill formed");
            return;
        }

        uint16_t lastHandle;
        if (!fromString(args[2], lastHandle)) {
            response->invalidParameters("The end handle for descriptors is ill formed");
            return;
        }

        if(startHandle >= lastHandle) {
            response->invalidParameters("start handle should not be greater or equal to last handle");
            return;
        }

        // if there is no descriptors to discover, just return an empty array
        if ((startHandle + 1) == lastHandle) {
            response->getResultStream() << serialization::startArray << serialization::endArray;
            response->success();
            return;
        }

        startProcedure<DiscoverAllCharacteristicsDescriptorsProcedure>(
            response, /* timeout */ 30 * 1000, connectionHandle, startHandle, lastHandle
        );
    }

    struct DiscoverAllCharacteristicsDescriptorsProcedure : public AsyncProcedure {
        DiscoverAllCharacteristicsDescriptorsProcedure(
            const SharedPointer<CommandResponse>& res,
            uint32_t timeout,
            uint16_t connectionHandle,
            uint16_t startHandle,
            uint16_t lastHandle
        ) : AsyncProcedure(res, timeout),
            characteristic(
                build_discovered_characteristic(connectionHandle, startHandle, lastHandle)
            ) {
        }

        virtual ~DiscoverAllCharacteristicsDescriptorsProcedure() {
            client().terminateCharacteristicDescriptorDiscovery(characteristic);
            gap().onDisconnection().detach(makeFunctionPointer(
                this, &DiscoverAllCharacteristicsDescriptorsProcedure::whenDisconnected
            ));
        }

        virtual bool doStart() {
            ble_error_t err = client().discoverCharacteristicDescriptors(
                characteristic,
                makeFunctionPointer(this, &DiscoverAllCharacteristicsDescriptorsProcedure::whenDescriptorDiscovered),
                makeFunctionPointer(this, &DiscoverAllCharacteristicsDescriptorsProcedure::whenServiceDiscoveryTerminated)
            );

            if(err) {
                response->faillure(err);
                return false;
            }

            gap().onDisconnection(makeFunctionPointer(
                this, &DiscoverAllCharacteristicsDescriptorsProcedure::whenDisconnected
            ));

            response->getResultStream() << serialization::startArray;
            return true;
        }

        void whenDescriptorDiscovered(const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t* result) {
            using namespace serialization;

            response->getResultStream() <<  startObject <<
                key("handle") << result->descriptor.getAttributeHandle() <<
                key("UUID") << result->descriptor.getUUID() <<
            endObject;
        }

        void whenServiceDiscoveryTerminated(const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t* params) {
            using namespace serialization;

            if(params->status) {
                response->getResultStream() << params->status << endArray;
                response->faillure();
            } else {
                response->getResultStream() << endArray;
                response->success();
            }

            terminate();
        }

        void whenDisconnected(const Gap::DisconnectionCallbackParams_t* params) {
            using namespace serialization;

            if(characteristic.getConnectionHandle() != params->handle) {
                return;
            };

            response->getResultStream() << "disconnection" << endArray;
            response->faillure();

            terminate();
        }

        virtual void doWhenTimeout() {
            using namespace serialization;

            response->getResultStream() << "discovery timeout" << endArray;
            response->faillure();
        }


        static DiscoveredCharacteristic build_discovered_characteristic(
            Gap::Handle_t conn,
            GattAttribute::Handle_t decl,
            GattAttribute::Handle_t last
        ) {
            struct DummyDiscoveredCharacteristic : public DiscoveredCharacteristic {
                DummyDiscoveredCharacteristic(
                    Gap::Handle_t conn,
                    GattAttribute::Handle_t decl,
                    GattAttribute::Handle_t last
                )  {
                    gattc = &client();
                    declHandle = decl;
                    valueHandle = decl + 1;
                    lastHandle = last;
                    connHandle = conn;
                }
            };

            return DummyDiscoveredCharacteristic(conn, decl, last);
        }

        DiscoveredCharacteristic characteristic;
    };
};


// this class handle the read procedure
struct ReadProcedure : public AsyncProcedure {
    /**
     * @brief Construct a read procedure, this will also attach all callbacks
     */
    ReadProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout, uint16_t _connectionHandle, uint16_t _valueHandle) :
        AsyncProcedure(res, timeout), connectionHandle(_connectionHandle), valueHandle(_valueHandle) {
    }

    virtual ~ReadProcedure() {
        // detach callbacks
        client().onDataRead().detach(makeFunctionPointer(this, &ReadProcedure::whenDataRead));
    }

    virtual bool doStart() {
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


struct ReadCharacteristicValueCommand : public BaseCommand {
    static const char* name() {
        return "readCharacteristicValue";
    }

    static const char* help() {
        return "Read a characteristic value from a GATT Server using a characteristic value handle.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandle>", "The handle of characteristic value" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

        startProcedure<ReadProcedure>(response, /* timeout */ 5 * 1000, connectionHandle, characteristicValueHandle);
    }
};


struct ReadUsingCharacteristicUUIDCommand : public BaseCommand {
    static const char* name() {
        return "readUsingCharacteristicUUID";
    }

    static const char* help() {
        return "This sub-procedure is used to read a Characteristic Value from a server "
               "when the client only knows the characteristic UUID and does not know the "
               "handle of the characteristic.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<serviceStartHandle>", "The starting handle of the service" },
            { "<serviceEndHandle>", "The ending handle of the service" },
            { "<characteristicUUID>", "The UUID of the characteristic" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct ReadLongCharacteristicValueCommand : public BaseCommand {
    static const char* name() {
        return "readLongCharacteristicValue";
    }

    static const char* help() {
        return "Read a characteristic value from a server when the client knows the "
               "characteristic value handle and the length of the characteristic value "
               "is longer than can be sent in a single read response";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandle>", "The handle of characteristic value" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct ReadMultipleCharacteristicValuesCommand : public BaseCommand {
    static const char* name() {
        return "readMultipleCharacteristicValues";
    }

    static const char* help() {
        return "Read a multiple characteristics values from a set of characteristics value handle.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandles...>", "Handles of characteristics values to read" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct WriteProcedure : public AsyncProcedure {
    WriteProcedure(const SharedPointer<CommandResponse>& res, uint32_t timeout,
        GattClient::WriteOp_t _cmd, uint16_t _connectionHandle, uint16_t _valueHandle, container::Vector<uint8_t> _dataToWrite) :
        AsyncProcedure(res, timeout), cmd(_cmd), connectionHandle(_connectionHandle),
        valueHandle(_valueHandle), dataToWrite(_dataToWrite) {
    }

    virtual ~WriteProcedure() {
        client().onDataWritten().detach(makeFunctionPointer(this, &WriteProcedure::whenDataWritten));
    }

    virtual bool doStart() {
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


struct WriteWithoutResponseCommand : public BaseCommand {
    static const char* name() {
        return "writeWithoutResponse";
    }

    static const char* help() {
        return "Write a characteristic value to a server, the server will not acknowledge anything.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
            { "<value>", "Hexadecimal string representation of the value to write" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

        container::Vector<uint8_t> dataToWrite = hexStringToRawData(args[2]);
        if(dataToWrite.size() == 0) {
            response->faillure("data to write provided are invalids");
            return;
        }

        startProcedure<WriteProcedure>(
            response, /* timeout */ 5 * 1000,
            GattClient::GATT_OP_WRITE_CMD, connectionHandle, characteristicValuehandle, dataToWrite
        );
    }
};


struct SignedWriteWithoutResponseCommand : public BaseCommand {
    static const char* name() {
        return "signedWriteWithoutResponse";
    }

    static const char* help() {
        return "Write a characteristic value to a server, the server will not acknowledge anything. "
               "This sub-procedure shall only be used if the CharacteristicProperties authenticated "
               "bit is enabled and the client and server device share a bond.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
            { "<value>", "Hexadecimal string representation of the value to write" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct WriteCommand : public BaseCommand {
    static const char* name() {
        return "write";
    }

    static const char* help() {
        return "Write a characteristic value to a server.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
            { "<value>", "Hexadecimal string representation of the value to write" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
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

        container::Vector<uint8_t> dataToWrite = hexStringToRawData(args[2]);
        if(dataToWrite.size() == 0) {
            response->faillure("data to write provided are invalids");
            return;
        }

        startProcedure<WriteProcedure>(
            response, 5 * 1000,
            GattClient::GATT_OP_WRITE_REQ, connectionHandle, characteristicValuehandle, dataToWrite
        );
    }
};


struct WriteLongCommand : public BaseCommand {
    static const char* name() {
        return "writeLong";
    }

    static const char* help() {
        return "Write a characteristic value to a server. This sub-procedure is used when "
               "the client knows the Characteristic Value Handle but the length of the "
               "Characteristic Value is longer than can be sent in a single Write Request "
               "Attribute Protocol message.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
            { "<value>", "Hexadecimal string representation of the value to write" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct ReliableWriteCommand : public BaseCommand {
    static const char* name() {
        return "reliableWrite";
    }

    static const char* help() {
        return "Write a characteristic value to a server. This sub-procedure is used when "
               "the client knows the Characteristic Value Handle, and assurance is required "
               "that the correct Characteristic Value is going to be written by transferring "
               "the Characteristic Value to be written in both directions before the write is "
               "performed. This sub-procedure can also be used when multiple values must be "
               "written, in order, in a single operation";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
            { "<value>", "Hexadecimal string representation of the value to write" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct ReadCharacteristicDescriptorCommand : public BaseCommand {
    static const char* name() {
        return "readCharacteristicDescriptor";
    }

    static const char* help() {
        return "Read a characteristic descriptor from a server.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to read" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->invalidParameters("the connection handle is ill formed");
            return;
        }

        uint16_t characteristicDescriptorHandle;
        if (!fromString(args[1], characteristicDescriptorHandle)) {
            response->invalidParameters("the characteristic value handle is ill formed");
            return;
        }

        startProcedure<ReadProcedure>(response, /* timeout */ 5 * 1000, connectionHandle, characteristicDescriptorHandle);
    }
};


struct ReadLongCharacteristicDescriptorCommand : public BaseCommand {
    static const char* name() {
        return "readLongCharacteristicDescriptor";
    }

    static const char* help() {
        return "Read a characteristic descriptor from a server. This procedure is used "
               "when the length of the characteristic descriptor declaration is longer "
               "than what can be sent in a single read";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to read" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};


struct WriteCharacteristicDescriptorCommand : public BaseCommand {
    static const char* name() {
        return "writeCharacteristicDescriptor";
    }

    static const char* help() {
        return "Write a characteristic descriptor to a server.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to write" },
            { "<value>", "Hexadecimal string representation of the value to write" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t connectionHandle;
        if (!fromString(args[0], connectionHandle)) {
            response->faillure("connection handle is ill formed");
            return;
        }

        uint16_t characteristicDescriptorhandle;
        if (!fromString(args[1], characteristicDescriptorhandle)) {
            response->faillure("characteristic value handle is ill formed");
            return;
        }

        container::Vector<uint8_t> dataToWrite = hexStringToRawData(args[2]);
        if(dataToWrite.size() == 0) {
            response->faillure("data to write provided are invalids");
            return;
        }

        startProcedure<WriteProcedure>(
            response, 5 * 1000,
            GattClient::GATT_OP_WRITE_REQ, connectionHandle, characteristicDescriptorhandle, dataToWrite
        );
    }
};


struct WriteLongCharacteristicDescriptorCommand : public BaseCommand {
    static const char* name() {
        return "writeLongCharacteristicDescriptor";
    }

    static const char* help() {
        return "Write a characteristic descriptor to a server. This procedure when the "
        "client knows that the length of the characteristic descriptor value is "
        "longer than what can be sent in a single write.";
    }

    static ConstArray<CommandArgDescription> argsDescription() {
        static const CommandArgDescription argsDescription[] = {
            { "<connectionHandle>", "The connection used by this procedure" },
            { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to write" },
            { "<value>", "Hexadecimal string representation of the value to write" }
        };
        return ConstArray<CommandArgDescription>(argsDescription);
    }

    static void handler(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        response->notImplemented();
    }
};

} // end of annonymous namespace

ConstArray<const Command*> GattClientCommandSuiteDescription::commands() {
    static const Command* const commandHandlers[] = {
        &CommandAccessor<DiscoverAllServicesAndCharacteristicsCommand>::command,
        &CommandAccessor<DiscoverAllServicesCommand>::command,
        &CommandAccessor<DiscoverPrimaryServicesByUUIDCommand>::command,
        //&CommandAccessor<DiscoverServicesCommand>::command,
        &CommandAccessor<FindIncludedServicesCommand>::command,
        &CommandAccessor<DiscoverCharacteristicsOfServiceCommand>::command,
        &CommandAccessor<DiscoverCharacteristicsByUUIDCommand>::command,
        &CommandAccessor<DiscoverAllCharacteristicsDescriptorsCommand>::command,
        &CommandAccessor<ReadCharacteristicValueCommand>::command,
        &CommandAccessor<ReadUsingCharacteristicUUIDCommand>::command,
        &CommandAccessor<ReadLongCharacteristicValueCommand>::command,
        &CommandAccessor<ReadMultipleCharacteristicValuesCommand>::command,
        &CommandAccessor<WriteWithoutResponseCommand>::command,
        &CommandAccessor<SignedWriteWithoutResponseCommand>::command,
        &CommandAccessor<WriteCommand>::command,
        &CommandAccessor<WriteLongCommand>::command,
        &CommandAccessor<ReliableWriteCommand>::command,
        &CommandAccessor<ReadCharacteristicDescriptorCommand>::command,
        &CommandAccessor<ReadLongCharacteristicDescriptorCommand>::command,
        &CommandAccessor<WriteCharacteristicDescriptorCommand>::command,
        &CommandAccessor<WriteLongCharacteristicDescriptorCommand>::command
    };

    return ConstArray<const Command*>(commandHandlers);
}
