#ifndef BLE_CLIAPP_BLE_COMMON_SERIALIZER_H_
#define BLE_CLIAPP_BLE_COMMON_SERIALIZER_H_

#include "Serializer.h"
#include "ble/blecommon.h"

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
		    { BLE_ERROR_UNSPECIFIED, "BLE_ERROR_UNSPECIFIED" }
	    };

	    return makeConstArray(map);
	}

	static const char* errorMessage() { 
		return "unknown ble_error_t";
	}
};

static inline const char* to_string(ble_error_t err) {
    switch(err) {
        case BLE_ERROR_NONE: 
            return "BLE_ERROR_NONE";
        case BLE_ERROR_BUFFER_OVERFLOW: 
            return "BLE_ERROR_BUFFER_OVERFLOW";
        case BLE_ERROR_NOT_IMPLEMENTED: 
            return "BLE_ERROR_NOT_IMPLEMENTED";
        case BLE_ERROR_PARAM_OUT_OF_RANGE: 
            return "BLE_ERROR_PARAM_OUT_OF_RANGE";
        case BLE_ERROR_INVALID_PARAM: 
            return "BLE_ERROR_INVALID_PARAM";
        case BLE_STACK_BUSY: 
            return "BLE_STACK_BUSY";
        case BLE_ERROR_INVALID_STATE: 
            return "BLE_ERROR_INVALID_STATE";
        case BLE_ERROR_NO_MEM: 
            return "BLE_ERROR_NO_MEM";
        case BLE_ERROR_OPERATION_NOT_PERMITTED: 
            return "BLE_ERROR_OPERATION_NOT_PERMITTED";
        case BLE_ERROR_UNSPECIFIED: 
            return "BLE_ERROR_UNSPECIFIED"; 
        default:
            return "Unknown ble_error_t";
    }
};

#endif //BLE_CLIAPP_BLE_COMMON_SERIALIZER_H_