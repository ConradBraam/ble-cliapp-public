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

