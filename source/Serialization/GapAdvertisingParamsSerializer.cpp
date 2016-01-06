#include "Serializer.h"
#include <stdint.h>
#include "ble/GapAdvertisingParams.h"

#include "GapAdvertisingParamsSerializer.h"

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GapAdvertisingParams& advertisingParams) {
    using namespace serialization;
    return os << startObject <<
        key("interval") << advertisingParams.getInterval() <<
        key("timeout") << advertisingParams.getTimeout() <<
        key("type") << advertisingParams.getAdvertisingType() <<
    endObject;
}

