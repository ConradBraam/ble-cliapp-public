#include "GattServerCommands.h"
#include "util/StaticLambda.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "Serialization/Serializer.h"
#include "Serialization/UUID.h"
#include "Serialization/Hex.h"
#include "Serialization/CharacteristicProperties.h"
#include "Serialization/BLECommonSerializer.h"

#include "util/ServiceBuilder.h"

template<typename T>
using SharedPointer = mbed::util::SharedPointer<T>;

// isolation
namespace {

static ServiceBuilder* serviceBuilder = nullptr;
static HeartRateService *HRMService = nullptr;

static BLE& ble() {
    return BLE::Instance();
}

static GattServer& gattServer() {
    return ble().gattServer();
}

static void cleanupServiceBuilder();

static void whenShutdown(const GattServer *) {
    cleanupServiceBuilder();
}

static void cleanupServiceBuilder() {
    if(!serviceBuilder) {
        return;
    }

    delete serviceBuilder;
    gattServer().onShutdown().detach(whenShutdown);
    serviceBuilder = nullptr;
}

static bool initServiceBuilder(const UUID& uuid) {
    if(serviceBuilder) {
        return false;
    }
    serviceBuilder = new ServiceBuilder(uuid);
    gattServer().onShutdown(whenShutdown);
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
        GattService* service = serviceBuilder->getService();

        ble_error_t err = gattServer().addService(*service);
        if(err) {
            response->faillure(err);
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
        cancelServiceDeclaration

    };

    return ConstArray<Command>(commandHandlers);
}
