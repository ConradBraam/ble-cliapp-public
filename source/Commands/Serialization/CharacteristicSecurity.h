#ifndef BLE_CLIAPP_CHARACTERISTIC_SECURITY_H_
#define BLE_CLIAPP_CHARACTERISTIC_SECURITY_H_

#include <stdint.h>

#include "Serialization/Serializer.h"
#include "ble/GattCharacteristic.h"
#include "Serialization/JSONOutputStream.h"

template<>
struct SerializerDescription<SecurityManager::SecurityMode_t> {
    typedef SecurityManager::SecurityMode_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { SecurityManager::SECURITY_MODE_NO_ACCESS, "SECURITY_MODE_NO_ACCESS" },
            { SecurityManager::SECURITY_MODE_ENCRYPTION_OPEN_LINK, "SECURITY_MODE_ENCRYPTION_OPEN_LINK" },
            { SecurityManager::SECURITY_MODE_ENCRYPTION_NO_MITM, "SECURITY_MODE_ENCRYPTION_NO_MITM" },
            { SecurityManager::SECURITY_MODE_ENCRYPTION_WITH_MITM, "SECURITY_MODE_ENCRYPTION_WITH_MITM" },
            { SecurityManager::SECURITY_MODE_SIGNED_NO_MITM, "SECURITY_MODE_SIGNED_NO_MITM" },
            { SecurityManager::SECURITY_MODE_SIGNED_WITH_MITM, "SECURITY_MODE_SIGNED_WITH_MITM" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown SecurityManager::SecurityMode_t";
    }
};

static inline serialization::JSONOutputStream& serializeCharacteristicSecurity(
    serialization::JSONOutputStream& os, SecurityManager::SecurityMode_t security) {
    using namespace serialization;

    os << toString(security);

    return os;
}

#endif //BLE_CLIAPP_CHARACTERISTIC_SECURITY_H_
