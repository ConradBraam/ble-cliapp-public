#include "GattServerCommands.h"
#include "util/StaticLambda.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/services/HeartRateService.h"
#include "Serialization/Serializer.h"
#include "Serialization/UUID.h"
#include "Serialization/Hex.h"
#include "Serialization/CharacteristicProperties.h"
#include "Serialization/BLECommonSerializer.h"
#include "Serialization/GattCallbackParamTypes.h"

#include "util/ServiceBuilder.h"
#include "util/AsyncProcedure.h"

template<typename T>
using SharedPointer = mbed::util::SharedPointer<T>;

// isolation
namespace {

static ServiceBuilder* serviceBuilder = nullptr;
static HeartRateService *HRMService = nullptr;

static detail::RAIIGattService** gattServices = nullptr;
static size_t gattServicesCount = 0;
static bool cleanupRegistered = false;

static BLE& ble() {
    return BLE::Instance();
}

static GattServer& gattServer() {
    return ble().gattServer();
}

static void cleanupServiceBuilder();

static void whenShutdown(const GattServer *) {
    cleanupServiceBuilder();
    for(size_t i = 0; i < gattServicesCount; ++i) {
        delete gattServices[i];
    }
    std::free(gattServices);
    gattServices = nullptr;
    gattServicesCount = 0;
    gattServer().onShutdown().detach(whenShutdown);
    cleanupRegistered = false;
}

static void cleanupServiceBuilder() {
    if(!serviceBuilder) {
        return;
    }

    delete serviceBuilder;
    serviceBuilder = nullptr;
}

static bool initServiceBuilder(const UUID& uuid) {
    if(serviceBuilder) {
        return false;
    }
    serviceBuilder = new ServiceBuilder(uuid);
    if(cleanupRegistered == false) {
        gattServer().onShutdown(whenShutdown);
        cleanupRegistered = true;
    }
    return true;
}

static constexpr const Command instantiateHRM {
    "instantiateHRM",
    "instantiate an HRM service, this command will be removed in the future",
    (const CommandArgDescription[]) {
        { "<value>", "The hrm sensor value (uint16_t)" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        if(HRMService) {
            response->faillure("The service has already been instantiated");
            return;
        }

        uint16_t sensorValue;
        if(!fromString(args[0], sensorValue)) {
            response->invalidParameters("The sensor value is ill formed");
            return;
        }

        HRMService = new HeartRateService(ble(), sensorValue, HeartRateService::LOCATION_FINGER);

        response->success();
    }
};

static constexpr const Command updateHRMSensorValue {
    "updateHRMSensorValue",
    "update the sensor value of the HRM service, this command will be removed in the future",
    (const CommandArgDescription[]) {
        { "<value>", "The new sensor value (uint16_t)" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        if(!HRMService) {
            response->faillure("The HRM service has not been instantiated");
            return;
        }

        uint16_t sensorValue;
        if(!fromString(args[0], sensorValue)) {
            response->invalidParameters("The sensor value is ill formed");
            return;
        }

        HRMService->updateHeartRate(sensorValue);

        response->success();
    }
};

static constexpr const Command declareService {
    "declareService",
    "Start the declaration of a service, after this call, user can call declareCharacteristic to declare "
    "a characteristic inside the service, commitService to commit the service or cancelServiceDeclaration "
    "to cancel the service declaration",
    (const CommandArgDescription[]) {
        { "<UUID>", "The UUID of the service" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        UUID serviceUUID;

        if(!fromString(args[0], serviceUUID)) {
            response->invalidParameters("The service UUID is ill formed");
            return;
        }

        if(initServiceBuilder(serviceUUID)) {
            response->success();
        } else {
            response->faillure("Impossible to start a service declaration, a service is already being declared");
        }
    }
};

static constexpr const Command declareCharacteristic {
    "declareCharacteristic",
    "Start the declaration of a characteristic, after this call, user can call declareCharacteristic to declare "
    "another characteristic inside the service, declareDescriptor to add a descriptor inside this characteristic, "
    "commitService to commit the service or cancelServiceDeclaration to cancel the service declaration",
    (const CommandArgDescription[]) {
        { "<UUID>", "The UUID of the characteristic" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        UUID characteristicUUID;

        if(!fromString(args[0], characteristicUUID)) {
            response->invalidParameters("The characteristic UUID is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        serviceBuilder->declareCharacteristic(characteristicUUID);
        response->success();
    }
};

static constexpr const Command setCharacteristicValue {
    "setCharacteristicValue",
    "Set the value of the characteristic being declared",
    (const CommandArgDescription[]) {
        { "<HexString>", "The value of the characteristic" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        container::Vector<std::uint8_t> characteristicValue = hexStringToRawData(args[0]);

        if(characteristicValue.size() == 0) {
            response->invalidParameters("The characteristic value is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        if(serviceBuilder->setCharacteristicValue(characteristicValue)) {
            response->success();
        } else {
            response->faillure("Impossible to set a characteristic value");
        }
    }
};

static constexpr const Command setCharacteristicProperties {
    "setCharacteristicProperties",
    "Set the properties of a characteristic being declared, this function expect a list of "
    "properties such as 'broadcast', 'read', 'writeWoResp', 'write', 'notify', 'indicate' and "
    "'authSignedWrite'",
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint8_t properties;
        if(!characteristicPropertiesFromStrings(args, properties)) {
            response->invalidParameters("Properties are ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        if(serviceBuilder->setCharacteristicProperties(properties)) {
            response->success();
        } else {
            response->faillure("Impossible to set the characteristic properties");
        }
    }
};

static constexpr const Command setCharacteristicVariableLength {
    "setCharacteristicVariableLength",
    "Set a boolean value which indicate if the characteristic has a variable len. If the "
    "characteristic has a variable len, max len could be set to bound the length to a maximum",
    (const CommandArgDescription[]) {
        { "<bool>", "The value of the variable length property" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        bool variableLen;
        if(!fromString(args[0], variableLen)) {
            response->invalidParameters("Variable len is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        if(serviceBuilder->setCharacteristicVariableLength(variableLen)) {
            response->success();
        } else {
            response->faillure("Impossible to set the characteristic variable length attribute");
        }
    }
};

static constexpr const Command setCharacteristicMaxLength {
    "setCharacteristicMaxLength",
    "Set the maximum lenght that is allowed for the value of the characteristic being declared",
    (const CommandArgDescription[]) {
        { "<uint16_t>", "Maximum length of the value of the characteristic being declared" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t maxLen;
        if(!fromString(args[0], maxLen)) {
            response->invalidParameters("Max len is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        if(serviceBuilder->setCharacteristicMaxLength(maxLen)) {
            response->success();
        } else {
            response->faillure("Impossible to set the characteristic maximum length");
        }
    }
};

static constexpr const Command declareDescriptor {
    "declareDescriptor",
    "Start the declaration of a descriptor which will be attached to the characteristic being declared",
    (const CommandArgDescription[]) {
        { "<UUID>", "The UUID of the descriptor" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        UUID descriptorUUID;

        if(!fromString(args[0], descriptorUUID)) {
            response->invalidParameters("The descriptor UUID is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }


        if(serviceBuilder->declareDescriptor(descriptorUUID)) {
            response->success();
        } else {
            response->faillure("Impossible to declare this descriptor");
        }
    }
};

static constexpr const Command setDescriptorValue {
    "setDescriptorValue",
    "Set the value of the descriptor being declared",
    (const CommandArgDescription[]) {
        { "<HexString>", "The value of the descriptor" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        container::Vector<std::uint8_t> descriptorValue = hexStringToRawData(args[0]);

        if(descriptorValue.size() == 0) {
            response->invalidParameters("The descriptor value is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        if(serviceBuilder->setDescriptorValue(descriptorValue)) {
            response->success();
        } else {
            response->faillure("Impossible to set the descriptor value");
        }
    }
};

static constexpr const Command setDescriptorVariableLength {
    "setDescriptorVariableLength",
    "Set a boolean value which indicate if the descriptor has a variable len. If the "
    "descriptor has a variable len, max len could be set to bound the length to a maximum",
    (const CommandArgDescription[]) {
        { "<bool>", "The value of the variable length property" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        bool variableLen;
        if(!fromString(args[0], variableLen)) {
            response->invalidParameters("Variable len is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        if(serviceBuilder->setDescriptorVariableLength(variableLen)) {
            response->success();
        } else {
            response->faillure("Impossible to set the descriptor variable length attribute");
        }
    }
};

static constexpr const Command setDescriptorMaxLength {
    "setDescriptorMaxLength",
    "Set the maximum lenght that is allowed for the value of the descriptor being declared",
    (const CommandArgDescription[]) {
        { "<uint16_t>", "Maximum length of the value of the descriptor being declared" }
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        uint16_t maxLen;
        if(!fromString(args[0], maxLen)) {
            response->invalidParameters("Max len is ill formed");
            return;
        }

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        if(serviceBuilder->setDescriptorMaxLength(maxLen)) {
            response->success();
        } else {
            response->faillure("Impossible to set the descriptor maximum length");
        }
    }
};

static constexpr const Command commitService {
    "commitService",
    "commit the service declaration",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        using namespace serialization;

        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }

        serviceBuilder->commit();
        detail::RAIIGattService* service = serviceBuilder->release();

        ble_error_t err = gattServer().addService(*service);
        if(err) {
            response->faillure(err);
            delete service;
        } else {
            response->success();
            // iterate over all handles
            auto& os = response->getResultStream() << startObject <<
                key("UUID") << service->getUUID() <<
                key("handle") << service->getHandle() <<
                key("characteristics") << startArray;

            for(uint16_t i = 0; i < service->getCharacteristicCount(); ++i) {
                GattCharacteristic& characteristic = *service->getCharacteristic(i);
                GattAttribute& characteristicAttribute = characteristic.getValueAttribute();

                os << startObject <<
                    key("UUID") << characteristicAttribute.getUUID() <<
                    key("value_handle") << characteristicAttribute.getHandle() <<
                    key("properties");  serializeCharacteristicProperties(os, characteristic.getProperties()) <<
                    key("length") << characteristicAttribute.getLength() <<
                    key("max_length") << characteristicAttribute.getMaxLength() <<
                    key("has_variable_length") << characteristicAttribute.hasVariableLength();

                if(characteristicAttribute.getLength()) {
                    os << key("value");
                    serializeRawDataToHexString(
                        os,
                        characteristicAttribute.getValuePtr(),
                        characteristicAttribute.getLength()
                    );
                } else {
                    os << key("value") << "";
                }

                os << key("descriptors") << startArray;
                for(uint16_t j = 0; j < characteristic.getDescriptorCount(); ++j) {
                    GattAttribute& descriptor = *characteristic.getDescriptor(j);
                    os << startObject <<
                        key("UUID") << descriptor.getUUID() <<
                        key("handle") << descriptor.getHandle() <<
                        key("length") << descriptor.getLength() <<
                        key("max_length") << descriptor.getMaxLength() <<
                        key("has_variable_length") << descriptor.hasVariableLength();

                    if(descriptor.getLength()) {
                        os << key("value");
                        serializeRawDataToHexString(
                            os,
                            descriptor.getValuePtr(),
                            descriptor.getLength()
                        );
                    } else {
                        os << key("value") << "";
                    }
                    os << endObject;
                }
                os << endArray;
                os << endObject;
            }
            os << endArray;
            os << endObject;

            // add the service inside the list of instantiated services
            gattServices = static_cast<detail::RAIIGattService**>(std::realloc(gattServices, sizeof(*gattServices) * (gattServicesCount + 1)));
            gattServices[gattServicesCount] = service;
            gattServicesCount += 1;

            // release unused memory
            service->releaseAttributesValue();
        }



        // anyway, everything is cleaned up
        cleanupServiceBuilder();
    }
};

static constexpr const Command cancelServiceDeclaration {
    "cancelServiceDeclaration",
    "cancel the service declaration",
    STATIC_LAMBDA(const CommandArgs&, const SharedPointer<CommandResponse>& response) {
        if(!serviceBuilder) {
            response->faillure("Their is no service being declared");
            return;
        }
        response->success();
        cleanupServiceBuilder();
    }
};

static constexpr const Command read {
    "read",
    "read the value of an attribute of the GATT server, this function take the"
    "attribute of the handle to read as first parameter. It is also possible to"
    "supply a connection handle has second parameter.",
    (const CommandArgDescription[]) {
        { "<uint16_t>", "The handle of the attribute to read" }
    },
    /* maximum args counts is two */ 2,
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        GattServer& server = gattServer();

        if(args.count() > 2) {
            response->invalidParameters("Too many arguments");
            return;
        }

        GattAttribute::Handle_t attributeHandle;
        if(!fromString(args[0], attributeHandle)) {
            response->invalidParameters("The attribute handle is ill formed");
            return;
        }

        if(args.count() == 2) {
            Gap::Handle_t connectionHandle;
            if(!fromString(args[1], connectionHandle)) {
                response->invalidParameters("The connection handle is ill formed");
                return;
            }

            uint16_t length = 0;
            ble_error_t err = server.read(connectionHandle, attributeHandle, nullptr, &length);
            if(err) {
                response->faillure(err);
                return;
            }

            uint8_t* buffer = new uint8_t[length];
            err = server.read(connectionHandle, attributeHandle, buffer, &length);
            if(err) {
                response->faillure(err);
            } else {
                serializeRawDataToHexString(response->getResultStream(), buffer, length);
                response->success();
            }
            delete[] buffer;
        } else {
            uint16_t length = 0;
            ble_error_t err = server.read(attributeHandle, nullptr, &length);
            if(err) {
                response->faillure(err);
                return;
            }

            uint8_t* buffer = new uint8_t[length];
            err = server.read(attributeHandle, buffer, &length);
            if(err) {
                response->faillure(err);
            } else {
                serializeRawDataToHexString(response->getResultStream(), buffer, length);
                response->success();
            }
            delete[] buffer;
        }
    }
};


static constexpr const Command write {
    "write",
    "write the value of an attribute of the GATT server, this function take the"
    "attribute of the handle to write as first parameter and the value to write "
    "as second parameter. It is also possible to supply a connection handle has "
    "third parameter.",
    (const CommandArgDescription[]) {
        { "<uint16_t>", "The handle of the attribute to write" },
        { "<HexString>", "The value to write" }
    },
    /* maximum args counts is two */ 3,
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        GattServer& server = gattServer();

        if(args.count() > 3) {
            response->invalidParameters("Too many arguments");
            return;
        }

        GattAttribute::Handle_t attributeHandle;
        if(!fromString(args[0], attributeHandle)) {
            response->invalidParameters("The attribute handle is ill formed");
            return;
        }

        auto value = hexStringToRawData(args[1]);
        if(value.size() == 0) {
            response->invalidParameters("The value to write is ill formed");
            return;
        }

        ble_error_t err = BLE_ERROR_UNSPECIFIED;

        if(args.count() == 3) {
            Gap::Handle_t connectionHandle;
            if(!fromString(args[2], connectionHandle)) {
                response->invalidParameters("The connection handle is ill formed");
                return;
            }

            err = server.write(connectionHandle, attributeHandle, value.begin(), value.size());
        } else {
            err = server.write(attributeHandle, value.begin(), value.size());
        }

        if(err) {
            response->faillure(err);
        } else {
            response->success();
        }
    }
};


static constexpr const Command waitForDataWritten {
    "waitForDataWritten",
    "Wait for a data to be written on a given characteristic from a given connection.",
    (const CommandArgDescription[]) {
        { "<uint16_t>", "The connection ID with the client supposed to write data" },
        { "<uint16_t>", "The attribute handle which will be written" },
        { "<timeout>", "Maximum time allowed for this procedure" },
    },
    STATIC_LAMBDA(const CommandArgs& args, const SharedPointer<CommandResponse>& response) {
        Gap::Handle_t connectionHandle;
        if(!fromString(args[0], connectionHandle)) {
            response->invalidParameters("The connection handle is ill formed");
            return;
        }

        GattAttribute::Handle_t attributeHandle;
        if(!fromString(args[1], attributeHandle)) {
            response->invalidParameters("The attribute handle is ill formed");
            return;
        }

        uint16_t procedureTimeout;
        if (!fromString(args[2], procedureTimeout)) {
            response->invalidParameters("the procedure timeout is ill formed");
            return;
        }

        struct WaitForDataWrittenProcedure : public AsyncProcedure {
            WaitForDataWrittenProcedure(const SharedPointer<CommandResponse>& res, uint32_t procedureTimeout,
                Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle) :
                AsyncProcedure(res, procedureTimeout),
                connection(connectionHandle),
                attribute(attributeHandle) {
            }

            virtual ~WaitForDataWrittenProcedure() {
                gattServer().onDataWritten().detach(makeFunctionPointer(this, &WaitForDataWrittenProcedure::whenDataWritten));
            }

            virtual bool doStart() {
                gattServer().onDataWritten(this, &WaitForDataWrittenProcedure::whenDataWritten);
                return true;
            }

            void whenDataWritten(const GattWriteCallbackParams* params) {
                // filter events not relevant
                if(params->connHandle != connection || params->handle != attribute) {
                    return;
                }

                response->success(*params);
                terminate();
            }

            Gap::Handle_t connection;
            GattAttribute::Handle_t attribute;
        };

        startProcedure<WaitForDataWrittenProcedure>(
            response,
            procedureTimeout,
            connectionHandle,
            attributeHandle
        );

    }
};

} // end of annonymous namespace

ConstArray<Command> GattServerCommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        instantiateHRM,
        updateHRMSensorValue,
        declareService,
        declareCharacteristic,
        setCharacteristicValue,
        setCharacteristicProperties,
        setCharacteristicVariableLength,
        setCharacteristicMaxLength,
        declareDescriptor,
        setDescriptorValue,
        setDescriptorVariableLength,
        setDescriptorMaxLength,
        commitService,
        cancelServiceDeclaration,
        read,
        write,
        waitForDataWritten
    };

    return ConstArray<Command>(commandHandlers);
}
