#include "GattCallbackParamTypes.h"
#include "Serialization/Hex.h"

dynamic::Value toDynamicValue(const GattReadCallbackParams* readResult) {
    dynamic::Value result;

    result["connection_handle"_ss] = (int64_t) readResult->connHandle;
    result["attribute_handle"_ss] = (int64_t) readResult->handle;
    result["offset"_ss] = (int64_t) readResult->offset;
    result["length"_ss] = (int64_t) readResult->len;
    result["data"_ss] = rawDataToHexString(readResult->data, readResult->len);

    return result;
}
