#ifndef BLE_CLIAPP_SECURITY_MANAGER_SERIALIZER_H_
#define BLE_CLIAPP_SECURITY_MANAGER_SERIALIZER_H_

#include <ble/SecurityManager.h>
#include "JSONOutputStream.h"
#include "Serializer.h"

template<>
struct SerializerDescription<SecurityManager::SecurityIOCapabilities_t> {
    typedef SecurityManager::SecurityIOCapabilities_t type;

    static const ConstArray<ValueToStringMapping<SecurityManager::SecurityIOCapabilities_t> > mapping() {
        static const ValueToStringMapping<SecurityManager::SecurityIOCapabilities_t> map[] = {
            { SecurityManager::IO_CAPS_DISPLAY_ONLY, "IO_CAPS_DISPLAY_ONLY" },
            { SecurityManager::IO_CAPS_DISPLAY_YESNO, "IO_CAPS_DISPLAY_YESNO" },
            { SecurityManager::IO_CAPS_KEYBOARD_ONLY, "IO_CAPS_KEYBOARD_ONLY" },
            { SecurityManager::IO_CAPS_NONE, "IO_CAPS_NONE" },
            { SecurityManager::IO_CAPS_KEYBOARD_DISPLAY, "IO_CAPS_KEYBOARD_DISPLAY" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown SecurityManager::SecurityIOCapabilities_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, SecurityManager::SecurityIOCapabilities_t type) {
    return os << toString(type);
}

#endif //BLE_CLIAPP_SECURITY_MANAGER_SERIALIZER_H_
