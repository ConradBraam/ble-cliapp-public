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

#include "Common.h"
#include "CLICommand/CommandHelper.h"


using mbed::util::SharedPointer;

// TODO: description of returned results

// isolation
namespace {

DECLARE_CMD(DiscoverAllServicesAndCharacteristicsCommand) {
    CMD_NAME("discoverAllServicesAndCharacteristics")

    CMD_HELP("Discover all services and characteristics available on a peer device")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure")
    )

    CMD_RESULTS(
        CMD_RESULT("JSON Array", "", "Array of the services discovered."),
        CMD_RESULT("JSON Object", "[i]", "A service"),
        CMD_RESULT("UUID", "[i].UUID", "UUID of the service"),
        CMD_RESULT("uint16_t", "[i].start_handle", "First handle of the service."),
        CMD_RESULT("uint16_t", "[i].end_handle", "Last handle of the service."),
        CMD_RESULT("JSON Array", "[i].characteristics", "Characteristics included in the service."),
        CMD_RESULT("UUID", "[i].characteristics[j].UUID", "UUID of the characteristic."),
        CMD_RESULT("JSON Array", "[i].characteristics[j].properties", "List of properties associated with the characteristic."),
        CMD_RESULT("uint16_t", "[i].characteristics[j].start_handle", "First handle of the characteristic."),
        CMD_RESULT("uint16_t", "[i].characteristics[j].value_handle", "Handle pointing to the value of the characteristic."),
        CMD_RESULT("uint16_t", "[i].characteristics[j].end_handle", "Last handle of the characteristic.")
    )

    CMD_HANDLER(uint16_t connectionHandle, CommandResponsePtr& response) {
        startProcedure<DiscoverAllServicesAndCharacteristicsProcedure>(
            response, /* timeout */ 30 * 1000, connectionHandle
        );
    }

    struct DiscoverAllServicesAndCharacteristicsProcedure : public AsyncProcedure {
        DiscoverAllServicesAndCharacteristicsProcedure(CommandResponsePtr& res, uint32_t timeout, uint16_t handle) :
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


DECLARE_CMD(DiscoverAllServicesCommand) {
    CMD_NAME("discoverAllServices")
    CMD_HELP("discover all services available on a peer device")
    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure")
    )

    CMD_RESULTS(
        CMD_RESULT("JSON Array", "", "Array of the services discovered."),
        CMD_RESULT("JSON Object", "[i]", "A service"),
        CMD_RESULT("UUID", "[i].UUID", "UUID of the service"),
        CMD_RESULT("uint16_t", "[i].start_handle", "First handle of the service."),
        CMD_RESULT("uint16_t", "[i].end_handle", "Last handle of the service.")
    )

    CMD_HANDLER(uint16_t connectionHandle, CommandResponsePtr& response) {
        startProcedure<DiscoverAllServicesProcedure>(
            response, /* timeout */ 30 * 1000, connectionHandle
        );
    }

    struct DiscoverAllServicesProcedure : public AsyncProcedure {
        DiscoverAllServicesProcedure(CommandResponsePtr& res, uint32_t timeout, uint16_t handle) :
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



DECLARE_CMD(DiscoverPrimaryServicesByUUIDCommand) {
    CMD_NAME("discoverPrimaryServicesByUUID")

    CMD_HELP("discover a specific kind of services on a peer device")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("UUID", "serviceUUID", "The UUID of the services to discover")
    )

    CMD_RESULTS(
        CMD_RESULT("JSON Array", "", "Array of the services discovered."),
        CMD_RESULT("JSON Object", "[i]", "A service"),
        CMD_RESULT("UUID", "[i].UUID", "UUID of the service"),
        CMD_RESULT("uint16_t", "[i].start_handle", "First handle of the service."),
        CMD_RESULT("uint16_t", "[i].end_handle", "Last handle of the service.")
    )

    CMD_HANDLER(uint16_t connectionHandle, UUID serviceUUID, CommandResponsePtr& response) {
        startProcedure<DiscoverServicesByUUIDProcedure>(
            response, /* timeout */ 30 * 1000, connectionHandle, serviceUUID
        );
    }

    struct DiscoverServicesByUUIDProcedure : public AsyncProcedure {
        DiscoverServicesByUUIDProcedure(CommandResponsePtr& res, uint32_t timeout, uint16_t handle, const UUID& uuid) :
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
    CMD_NAME("findIncludedServices")

    CMD_HELP("Find includded services declaration withn a service definition on the server")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "serviceStartHandle", "The starting handle of the service" ),
        CMD_ARG("uint16_t", "serviceEndHandle", "The ending handle of the service")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};



struct DiscoverCharacteristicsOfServiceCommand : public BaseCommand {
    CMD_NAME("discoverCharacteristicsOfService")

    CMD_HELP("Discover all characteristics of a service, this procedure will find all the"
               "characteristics declaration within a service definition on a server"
    )

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "serviceStartHandle", "The starting handle of the service" ),
        CMD_ARG("uint16_t", "serviceEndHandle", "The ending handle of the service")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct DiscoverCharacteristicsByUUIDCommand : public BaseCommand {
    CMD_NAME("discoverCharacteristicsByUUID")

    CMD_HELP("Discover all characteristics of a service matching a specific UUID.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "serviceStartHandle", "The starting handle of the service" ),
        CMD_ARG("uint16_t", "serviceEndHandle", "The ending handle of the service" ),
        CMD_ARG("UUID", "serviceUUID", "The UUID of the characteristics to discover")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct DiscoverAllCharacteristicsDescriptorsCommand : public BaseCommand {
    CMD_NAME("discoverAllCharacteristicsDescriptors")

    CMD_HELP("Find all the characteristic descriptor’s Attribute Handles and Attribute "
               "Types within a characteristic definition. The characteristic specified is "
               "identified by the characteristic handle range.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicStartHandle", "The start handle of of the characteristic"),
        CMD_ARG("uint16_t", "endHandle", "The ending handle of the characteristic definition")
    )

    CMD_RESULTS(
        CMD_RESULT("JSON Array", "", "Array of the descriptors discovered."),
        CMD_RESULT("UUID", "[i].UUID", "UUID of the descriptor"),
        CMD_RESULT("uint16_t", "[i].handle", "Handle of the descriptor.")
    )

    CMD_HANDLER(uint16_t connectionHandle, uint16_t startHandle, uint16_t lastHandle, CommandResponsePtr& response) {
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
            CommandResponsePtr& res,
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
    ReadProcedure(CommandResponsePtr& res, uint32_t timeout, uint16_t _connectionHandle, uint16_t _valueHandle) :
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
    CMD_NAME("readCharacteristicValue")

    CMD_HELP("Read a characteristic value from a GATT Server using a characteristic value handle.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandle", "The handle of characteristic value")
    )

    CMD_RESULTS(
        CMD_RESULT("HexString_t", "", "The data read")
    )

    CMD_HANDLER(uint16_t connectionHandle, uint16_t characteristicValueHandle, CommandResponsePtr& response) {
        startProcedure<ReadProcedure>(response, /* timeout */ 5 * 1000, connectionHandle, characteristicValueHandle);
    }
};


struct ReadUsingCharacteristicUUIDCommand : public BaseCommand {
    CMD_NAME("readUsingCharacteristicUUID")

    CMD_HELP("This sub-procedure is used to read a Characteristic Value from a server "
               "when the client only knows the characteristic UUID and does not know the "
               "handle of the characteristic.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "serviceStartHandle", "The starting handle of the service" ),
        CMD_ARG("uint16_t", "serviceEndHandle", "The ending handle of the service" ),
        CMD_ARG("UUID", "characteristicUUID", "The UUID of the characteristic")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct ReadLongCharacteristicValueCommand : public BaseCommand {
    CMD_NAME("readLongCharacteristicValue")

    CMD_HELP("Read a characteristic value from a server when the client knows the "
               "characteristic value handle and the length of the characteristic value "
               "is longer than can be sent in a single read response")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandle", "The handle of characteristic value")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct ReadMultipleCharacteristicValuesCommand : public BaseCommand {
    CMD_NAME("readMultipleCharacteristicValues")

    CMD_HELP("Read a multiple characteristics values from a set of characteristics value handle.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandles...", "Handles of characteristics values to read")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct WriteProcedure : public AsyncProcedure {
    WriteProcedure(CommandResponsePtr& res, uint32_t timeout,
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
    CMD_NAME("writeWithoutResponse")

    CMD_HELP("Write a characteristic value to a server, the server will not acknowledge anything.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandle", "Handle of the characteristic value to write" ),
        CMD_ARG("RawData_t", "value", "Hexadecimal string representation of the value to write")
    )

    CMD_HANDLER(uint16_t connectionHandle, uint16_t characteristicValuehandle, container::Vector<uint8_t>& dataToWrite, CommandResponsePtr& response) {
        startProcedure<WriteProcedure>(
            response, /* timeout */ 5 * 1000,
            GattClient::GATT_OP_WRITE_CMD, connectionHandle, characteristicValuehandle, dataToWrite
        );
    }
};


struct SignedWriteWithoutResponseCommand : public BaseCommand {
    CMD_NAME("signedWriteWithoutResponse")

    CMD_HELP("Write a characteristic value to a server, the server will not acknowledge anything. "
               "This sub-procedure shall only be used if the CharacteristicProperties authenticated "
               "bit is enabled and the client and server device share a bond.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandle", "Handle of the characteristic value to write" ),
        CMD_ARG("RawData_t", "value", "Hexadecimal string representation of the value to write")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct WriteCommand : public BaseCommand {
    CMD_NAME("write")

    CMD_HELP("Write a characteristic value to a server.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandle", "Handle of the characteristic value to write" ),
        CMD_ARG("RawData_t", "value", "Hexadecimal string representation of the value to write")
    )

    CMD_HANDLER(uint16_t connectionHandle, uint16_t characteristicValuehandle, RawData_t dataToWrite, CommandResponsePtr& response) {
        startProcedure<WriteProcedure>(
            response, 5 * 1000,
            GattClient::GATT_OP_WRITE_REQ, connectionHandle, characteristicValuehandle, dataToWrite
        );
    }
};


struct WriteLongCommand : public BaseCommand {
    CMD_NAME("writeLong")

    CMD_HELP("Write a characteristic value to a server. This sub-procedure is used when "
               "the client knows the Characteristic Value Handle but the length of the "
               "Characteristic Value is longer than can be sent in a single Write Request "
               "Attribute Protocol message.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandle", "Handle of the characteristic value to write" ),
        CMD_ARG("RawData_t", "value", "Hexadecimal string representation of the value to write")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct ReliableWriteCommand : public BaseCommand {
    CMD_NAME("reliableWrite")

    CMD_HELP("Write a characteristic value to a server. This sub-procedure is used when "
               "the client knows the Characteristic Value Handle, and assurance is required "
               "that the correct Characteristic Value is going to be written by transferring "
               "the Characteristic Value to be written in both directions before the write is "
               "performed. This sub-procedure can also be used when multiple values must be "
               "written, in order, in a single operation")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicValuehandle", "Handle of the characteristic value to write" ),
        CMD_ARG("RawData_t", "value", "Hexadecimal string representation of the value to write")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct ReadCharacteristicDescriptorCommand : public BaseCommand {
    CMD_NAME("readCharacteristicDescriptor")

    CMD_HELP("Read a characteristic descriptor from a server.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicDescriptorhandle", "Handle of the characteristic descriptor to read")
    )

    CMD_HANDLER(uint16_t connectionHandle, uint16_t characteristicDescriptorHandle, CommandResponsePtr& response) {
        startProcedure<ReadProcedure>(response, /* timeout */ 5 * 1000, connectionHandle, characteristicDescriptorHandle);
    }
};


struct ReadLongCharacteristicDescriptorCommand : public BaseCommand {
    CMD_NAME("readLongCharacteristicDescriptor")

    CMD_HELP(
        "Read a characteristic descriptor from a server. This procedure is used "
        "when the length of the characteristic descriptor declaration is longer "
        "than what can be sent in a single read"
    )

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicDescriptorhandle", "Handle of the characteristic descriptor to read")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};


struct WriteCharacteristicDescriptorCommand : public BaseCommand {
    CMD_NAME("writeCharacteristicDescriptor")

    CMD_HELP("Write a characteristic descriptor to a server.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicDescriptorhandle", "Handle of the characteristic descriptor to write" ),
        CMD_ARG("RawData_t", "value", "Hexadecimal string representation of the value to write")
    )

    CMD_HANDLER(uint16_t connectionHandle, uint16_t characteristicDescriptorhandle, RawData_t dataToWrite, CommandResponsePtr& response) {
        startProcedure<WriteProcedure>(
            response, 5 * 1000,
            GattClient::GATT_OP_WRITE_REQ, connectionHandle, characteristicDescriptorhandle, dataToWrite
        );
    }
};


struct WriteLongCharacteristicDescriptorCommand : public BaseCommand {
    CMD_NAME("writeLongCharacteristicDescriptor")

    CMD_HELP("Write a characteristic descriptor to a server. This procedure when the "
        "client knows that the length of the characteristic descriptor value is "
        "longer than what can be sent in a single write.")

    CMD_ARGS(
        CMD_ARG("uint16_t", "connectionHandle", "The connection used by this procedure" ),
        CMD_ARG("uint16_t", "characteristicDescriptorhandle", "Handle of the characteristic descriptor to write" ),
        CMD_ARG("RawData_t", "value", "Hexadecimal string representation of the value to write")
    )

    CMD_HANDLER(const CommandArgs&, CommandResponsePtr& response) {
        response->notImplemented();
    }
};

} // end of annonymous namespace


DECLARE_SUITE_COMMANDS(GattClientCommandSuiteDescription, 
    CMD_INSTANCE(DiscoverAllServicesAndCharacteristicsCommand),
    CMD_INSTANCE(DiscoverAllServicesCommand),
    CMD_INSTANCE(DiscoverPrimaryServicesByUUIDCommand),
    //CMD_INSTANCE(DiscoverServicesCommand),
    CMD_INSTANCE(FindIncludedServicesCommand),
    CMD_INSTANCE(DiscoverCharacteristicsOfServiceCommand),
    CMD_INSTANCE(DiscoverCharacteristicsByUUIDCommand),
    CMD_INSTANCE(DiscoverAllCharacteristicsDescriptorsCommand),
    CMD_INSTANCE(ReadCharacteristicValueCommand),
    CMD_INSTANCE(ReadUsingCharacteristicUUIDCommand),
    CMD_INSTANCE(ReadLongCharacteristicValueCommand),
    CMD_INSTANCE(ReadMultipleCharacteristicValuesCommand),
    CMD_INSTANCE(WriteWithoutResponseCommand),
    CMD_INSTANCE(SignedWriteWithoutResponseCommand),
    CMD_INSTANCE(WriteCommand),
    CMD_INSTANCE(WriteLongCommand),
    CMD_INSTANCE(ReliableWriteCommand),
    CMD_INSTANCE(ReadCharacteristicDescriptorCommand),
    CMD_INSTANCE(ReadLongCharacteristicDescriptorCommand),
    CMD_INSTANCE(WriteCharacteristicDescriptorCommand),
    CMD_INSTANCE(WriteLongCharacteristicDescriptorCommand)
)
