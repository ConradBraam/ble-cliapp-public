#ifndef BLE_CLIAPP_SECURITY_MANAGER_SERIALIZER_H_
#define BLE_CLIAPP_SECURITY_MANAGER_SERIALIZER_H_

#include <ble/SecurityManager.h>
#include "Serialization/JSONOutputStream.h"
#include "Serialization/Serializer.h"

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

template<>
struct SerializerDescription<SecurityManager::SecurityCompletionStatus_t> {
    typedef SecurityManager::SecurityCompletionStatus_t type;

    static const ConstArray<ValueToStringMapping<SecurityManager::SecurityCompletionStatus_t> > mapping() {
        static const ValueToStringMapping<SecurityManager::SecurityCompletionStatus_t> map[] = {
            { SecurityManager::SEC_STATUS_SUCCESS, "SEC_STATUS_SUCCESS" },
            { SecurityManager::SEC_STATUS_TIMEOUT, "SEC_STATUS_TIMEOUT" },
            { SecurityManager::SEC_STATUS_PDU_INVALID, "SEC_STATUS_PDU_INVALID" },
            { SecurityManager::SEC_STATUS_PASSKEY_ENTRY_FAILED, "SEC_STATUS_PASSKEY_ENTRY_FAILED" },
            { SecurityManager::SEC_STATUS_OOB_NOT_AVAILABLE, "SEC_STATUS_OOB_NOT_AVAILABLE" },
            { SecurityManager::SEC_STATUS_AUTH_REQ, "SEC_STATUS_AUTH_REQ" },
            { SecurityManager::SEC_STATUS_CONFIRM_VALUE, "SEC_STATUS_CONFIRM_VALUE" },
            { SecurityManager::SEC_STATUS_PAIRING_NOT_SUPP, "SEC_STATUS_PAIRING_NOT_SUPP" },
            { SecurityManager::SEC_STATUS_ENC_KEY_SIZE, "SEC_STATUS_ENC_KEY_SIZE" },
            { SecurityManager::SEC_STATUS_SMP_CMD_UNSUPPORTED, "SEC_STATUS_SMP_CMD_UNSUPPORTED" },
            { SecurityManager::SEC_STATUS_UNSPECIFIED, "SEC_STATUS_UNSPECIFIED" },
            { SecurityManager::SEC_STATUS_REPEATED_ATTEMPTS, "SEC_STATUS_REPEATED_ATTEMPTS" },
            { SecurityManager::SEC_STATUS_INVALID_PARAMS, "SEC_STATUS_INVALID_PARAMS" },
            { SecurityManager::SEC_STATUS_DHKEY_CHECK_FAILED, "SEC_STATUS_DHKEY_CHECK_FAILED" },
            { SecurityManager::SEC_STATUS_COMPARISON_FAILED, "SEC_STATUS_COMPARISON_FAILED" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown SecurityManager::SecurityCompletionStatus_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, SecurityManager::SecurityCompletionStatus_t type) {
    return os << toString(type);
}

#endif //BLE_CLIAPP_SECURITY_MANAGER_SERIALIZER_H_
