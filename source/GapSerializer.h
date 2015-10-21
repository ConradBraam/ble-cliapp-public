#ifndef BLE_CLIAPP_GAP_SERIALIZER_H_
#define BLE_CLIAPP_GAP_SERIALIZER_H_

#include "Serializer.h"
#include "ble/Gap.h"
#include "picojson.h"


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




struct MacAddressString_t { 
	char str[sizeof("XX:XX:XX:XX:XX:XX")];
};


// MAC address serializer/deserializer
static inline bool macAddressFromString(const char* str, Gap::Address_t& val) {
	// since nano libc does not properly handle all format flags, we rely on integers for sscanf
	int tmp[Gap::ADDR_LEN];

	int count = sscanf(str, "%x:%x:%x:%x:%x:%x", &tmp[5], &tmp[4], &tmp[3], &tmp[2], &tmp[1], &tmp[0]);

	if(count == Gap::ADDR_LEN) {
		for(size_t i = 0; i < Gap::ADDR_LEN; ++i) {
			val[i] = (uint8_t) tmp[i];			
		}
		return true;
	}

	return false;
}

static inline MacAddressString_t macAddressToString(const Gap::Address_t& src) {
	MacAddressString_t converted;
	snprintf(converted.str, sizeof(converted.str), "%02X:%02X:%02X:%02X:%02X:%02X", src[5], src[4], src[3],src[2],src[1],src[0]);
	return converted;
}



// TODO : bidirectional way of serialization / deserialization 

/// convert Gap::ConnectionParams_t from CLI to structure
static inline bool connectionParamsFromCLI(const char* str, Gap::ConnectionParams_t& params) {
	// since nano libc does not properly handle all format flags, we rely on integers for sscanf
	unsigned int tmp[4];

	int count = sscanf(str, "%u,%u,%u,%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);

	if(count == 4) {
        params.minConnectionInterval = tmp[0];
        params.maxConnectionInterval = tmp[1];
        params.slaveLatency = tmp[2];
        params.connectionSupervisionTimeout = tmp[3];
        return true;
	}

	return false;
}

static inline picojson::value connectionParamsToJSON(const Gap::ConnectionParams_t& params) {
	picojson::value res(picojson::object_type, true);
	picojson::object& obj = res.get<picojson::object>();

	obj["minConnectionInterval"] = picojson::value((int64_t) params.minConnectionInterval);
	obj["maxConnectionInterval"] = picojson::value((int64_t) params.maxConnectionInterval);
	obj["slaveLatency"] = picojson::value((int64_t) params.slaveLatency);
	obj["connectionSupervisionTimeout"] = picojson::value((int64_t) params.connectionSupervisionTimeout);
	return res;
}








#endif //BLE_CLIAPP_GAP_SERIALIZER_H_