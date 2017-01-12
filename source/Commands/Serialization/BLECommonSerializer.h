#ifndef BLE_CLIAPP_BLE_COMMON_SERIALIZER_H_
#define BLE_CLIAPP_BLE_COMMON_SERIALIZER_H_

#include "Serialization/Serializer.h"
#include "ble/blecommon.h"
#include "Serialization/JSONOutputStream.h"

template<>
struct SerializerDescription<ble_error_t> {
    typedef ble_error_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { BLE_ERROR_NONE, "BLE_ERROR_NONE" },
            { BLE_ERROR_BUFFER_OVERFLOW, "BLE_ERROR_BUFFER_OVERFLOW" },
            { BLE_ERROR_NOT_IMPLEMENTED, "BLE_ERROR_NOT_IMPLEMENTED" },
            { BLE_ERROR_PARAM_OUT_OF_RANGE, "BLE_ERROR_PARAM_OUT_OF_RANGE" },
            { BLE_ERROR_INVALID_PARAM, "BLE_ERROR_INVALID_PARAM" },
            { BLE_STACK_BUSY, "BLE_STACK_BUSY" },
            { BLE_ERROR_INVALID_STATE, "BLE_ERROR_INVALID_STATE" },
            { BLE_ERROR_NO_MEM, "BLE_ERROR_NO_MEM" },
            { BLE_ERROR_OPERATION_NOT_PERMITTED, "BLE_ERROR_OPERATION_NOT_PERMITTED" },
            { BLE_ERROR_INITIALIZATION_INCOMPLETE, "BLE_ERROR_INITIALIZATION_INCOMPLETE" },
            { BLE_ERROR_ALREADY_INITIALIZED, "BLE_ERROR_ALREADY_INITIALIZED " },
            { BLE_ERROR_UNSPECIFIED, "BLE_ERROR_UNSPECIFIED" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble_error_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, ble_error_t err) {
    return os << toString(err);
}

#endif //BLE_CLIAPP_BLE_COMMON_SERIALIZER_H_
