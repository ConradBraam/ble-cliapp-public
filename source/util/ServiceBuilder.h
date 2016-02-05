#ifndef BLE_CLIAPP_UTIL_SERVICE_BUILDER_
#define BLE_CLIAPP_UTIL_SERVICE_BUILDER_

#include "ble/GattService.h"
#include "ble/GattCharacteristic.h"
#include "ble/GattAttribute.h"
#include "ble/UUID.h"
#include "ble/GattServer.h"
#include "detail/RAIIGattAttribute.h"
#include "detail/RAIIGattCharacteristic.h"
#include "detail/RAIIGattService.h"

class ServiceBuilder {

public:
    ServiceBuilder(const UUID& uuid) :
        service(uuid), currentCharacteristic(nullptr), currentDescriptor(nullptr) {
    }

    ServiceBuilder(const ServiceBuilder&) = delete;
    ServiceBuilder& operator=(const ServiceBuilder&) = delete;

    void declareCharacteristic(const UUID& characteristicUUID) {
        commit();
        currentCharacteristic = new detail::RAIIGattCharacteristic(characteristicUUID);
    }

    bool setCharacteristicValue(const container::Vector<std::uint8_t>& characteristicValue) {
        if(!currentCharacteristic) {
            return false;
        }
        currentCharacteristic->setValue(characteristicValue);
        return true;
    }

    bool setCharacteristicProperties(std::uint8_t properties) {
        if(!currentCharacteristic) {
            return false;
        }
        currentCharacteristic->setProperties(properties);
        return true;
    }

    bool setCharacteristicVariableLength(bool variableLen) {
        if(!currentCharacteristic) {
            return false;
        }
        currentCharacteristic->setVariableLength(variableLen);
        return true;
    }

    bool setCharacteristicMaxLength(std::uint16_t maxLen) {
        if(!currentCharacteristic) {
            return false;
        }
        return currentCharacteristic->setMaxLength(maxLen);
    }

    bool declareDescriptor(const UUID& descriptorUUID) {
        if(!currentCharacteristic) {
            return false;
        }

        if(currentDescriptor) {
            currentCharacteristic->addDescriptor(currentDescriptor);
        }

        currentDescriptor = new detail::RAIIGattAttribute(descriptorUUID);
        return true;
    }

    bool setDescriptorValue(const container::Vector<std::uint8_t>& descriptorValue) {
        if(!currentCharacteristic || !currentDescriptor) {
            return false;
        }

        currentDescriptor->setValue(descriptorValue);
        return true;
    }

    bool setDescriptorVariableLength(bool variableLen) {
        if(!currentCharacteristic || !currentDescriptor) {
            return false;
        }

        currentDescriptor->setVariableLength(variableLen);
        return true;
    }

    bool setDescriptorMaxLength(std::uint16_t maxLen) {
        if(!currentCharacteristic || !currentDescriptor) {
            return false;
        }

        return currentDescriptor->setMaxLength(maxLen);
    }

    GattService* getService() {
        return &service;
    }

    void commit() {
        if(currentCharacteristic) {
            if(currentDescriptor) {
                currentCharacteristic->addDescriptor(currentDescriptor);
                currentDescriptor = nullptr;
            }
            service.addCharacteristic(currentCharacteristic);
            currentCharacteristic = nullptr;
        }
    }

private:
    detail::RAIIGattService service;
    detail::RAIIGattCharacteristic* currentCharacteristic;
    detail::RAIIGattAttribute* currentDescriptor;

};


#endif //BLE_CLIAPP_UTIL_SERVICE_BUILDER_
