#ifndef BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_
#define BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_

#include "Serializer.h"
#include "ble/GapAdvertisingParams.h"
#include "dynamic/Value.h"
#include "util/DynamicString.h"

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

static inline dynamic::Value advertisingParamsToJSON(const GapAdvertisingParams& advertisingParams) { 
    dynamic::Value result;
    result["interval"_ss] = (int64_t) advertisingParams.getInterval();
    result["timeout"_ss] = (int64_t) advertisingParams.getTimeout();
    result["type"_ss] = toString(advertisingParams.getAdvertisingType());
    return result;
}

#endif //BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_