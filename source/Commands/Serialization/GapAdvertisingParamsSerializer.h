#ifndef BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_
#define BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_

#include "Serialization/Serializer.h"
#include "ble/GapAdvertisingParams.h"
#include "Serialization/JSONOutputStream.h"

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

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, GapAdvertisingParams::AdvertisingType_t type) {
    return os << toString(type);
}

/**
 * @brief Convert an advertising param to a dynamic::Value

 * @param advertisingParams The advertising param to convert
 */
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GapAdvertisingParams& advertisingParams);


#endif //BLE_CLIAPP_GAP_ADVERTISING_PARAMS_SERIALIZER_H_
