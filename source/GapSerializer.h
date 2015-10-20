#ifndef BLE_CLIAPP_GAP_SERIALIZER_H_
#define BLE_CLIAPP_GAP_SERIALIZER_H_

#include "Serializer.h"
#include "ble/Gap.h"


template<>
struct SerializerDescription<Gap::AddressType_t> {
	typedef Gap::AddressType_t type;

	static const ConstArray<ValueToStringMapping<Gap::AddressType_t> > mapping() {
	    static const ValueToStringMapping<Gap::AddressType_t> map[] = { 
	    	{ Gap::ADDR_TYPE_PUBLIC, "ADDR_TYPE_PUBLIC" },
	        { Gap::ADDR_TYPE_RANDOM_STATIC, "ADDR_TYPE_RANDOM_STATIC" },
	        { Gap::ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE, "ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE" },
	        { Gap::ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE, "ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE" }
	    };

	    return makeConstArray(map);
	}

	static const char* errorMessage() { 
		return "unknown Gap::AddressType_t";
	}
};




#endif //BLE_CLIAPP_GAP_SERIALIZER_H_