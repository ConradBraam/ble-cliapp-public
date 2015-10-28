#ifndef BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_
#define BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_

#include "Serializer.h"
#include "util/picojson.h"
#include "ble/GapAdvertisingParams.h"


template<>
struct SerializerDescription<GapAdvertisingParams::AdvertisingType_t> {
	typedef GapAdvertisingParams::AdvertisingType_t type;

	static const ConstArray<ValueToStringMapping<GapAdvertisingParams::AdvertisingType_t> > mapping() {
	    static const ValueToStringMapping<GapAdvertisingParams::AdvertisingType_t> map[] = { 
			{ GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED, "ADV_CONNECTABLE_UNDIRECTED" },
			{ GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED, "ADV_CONNECTABLE_DIRECTED" },
			{ GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED, "ADV_SCANNABLE_UNDIRECTED" },
			{ GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED, "ADV_NON_CONNECTABLE_UNDIRECTED" } 
	    };

	    return makeConstArray(map);
	}

	static const char* errorMessage() { 
		return "unknown GapAdvertisingParams::AdvertisingType_t";
	}
};


static inline picojson::value advertisingParamsToJSON(const GapAdvertisingParams& advertisingParams) { 
	picojson::value result(picojson::object_type, true);
	result.get<picojson::object>()["interval"] = picojson::value((int64_t) advertisingParams.getInterval());
	result.get<picojson::object>()["timeout"] = picojson::value((int64_t) advertisingParams.getTimeout());
	result.get<picojson::object>()["type"] = picojson::value(toString(advertisingParams.getAdvertisingType()));
	return result;
}



#endif //BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_