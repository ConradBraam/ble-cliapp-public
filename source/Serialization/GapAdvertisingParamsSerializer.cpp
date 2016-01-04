#include "Serializer.h"
#include "ble/GapAdvertisingParams.h"
#include "dynamic/Value.h"
#include "util/DynamicString.h"

#include "GapAdvertisingParamsSerializer.h"

dynamic::Value advertisingParamsToJSON(const GapAdvertisingParams& advertisingParams) {
    dynamic::Value result;
    result["interval"_ss] = (int64_t) advertisingParams.getInterval();
    result["timeout"_ss] = (int64_t) advertisingParams.getTimeout();
    result["type"_ss] = toString(advertisingParams.getAdvertisingType());
    return result;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GapAdvertisingParams& advertisingParams) {
    using namespace serialization;
    return os << startObject <<
        key("interval") << advertisingParams.getInterval() <<
        key("timeout") << advertisingParams.getTimeout() <<
        key("type") << advertisingParams.getAdvertisingType() <<
    endObject;

}

