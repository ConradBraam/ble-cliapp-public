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

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GattReadCallbackParams& readResult) {
    using namespace serialization;
    os << startObject <<
        key("connection_handle") << readResult.connHandle <<
        key("attribute_handle") << readResult.handle <<
        key("offset") << readResult.offset <<
        key("length") << readResult.len <<
        key("data");
    return serializeRawDataToHexString(os, readResult.data, readResult.len) << endObject;
}


namespace {
const char* toString(GattWriteCallbackParams::WriteOp_t op) {
    switch(op) {
        case GattWriteCallbackParams::OP_INVALID:
            return "OP_INVALID";
        case GattWriteCallbackParams::OP_WRITE_REQ:
            return "OP_WRITE_REQ";
        case GattWriteCallbackParams::OP_WRITE_CMD:
            return "OP_WRITE_CMD";
        case GattWriteCallbackParams::OP_SIGN_WRITE_CMD:
            return "OP_SIGN_WRITE_CMD";
        case GattWriteCallbackParams::OP_PREP_WRITE_REQ:
            return "OP_PREP_WRITE_REQ";
        case GattWriteCallbackParams::OP_EXEC_WRITE_REQ_CANCEL:
            return "OP_EXEC_WRITE_REQ_CANCEL";
        case GattWriteCallbackParams::OP_EXEC_WRITE_REQ_NOW:
            return "OP_EXEC_WRITE_REQ_NOW";
        default:
            return "invalid GattWriteCallbackParams::WriteOp_t operation";
    }
}
}

dynamic::Value toDynamicValue(const GattWriteCallbackParams* writeResult) {
    dynamic::Value result;

    result["connection_handle"_ss] = (int64_t) writeResult->connHandle;
    result["attribute_handle"_ss] = (int64_t) writeResult->handle;
    result["offset"_ss] = (int64_t) writeResult->offset;
    result["length"_ss] = (int64_t) writeResult->len;
    result["data"_ss] = rawDataToHexString(writeResult->data, writeResult->len);
    result["write_operation_type"_ss] = toString(writeResult->writeOp);

    return result;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GattWriteCallbackParams& writeResult) {
    using namespace serialization;

    os << startObject <<
        key("connection_handle") << writeResult.connHandle <<
        key("attribute_handle") << writeResult.handle <<
        key("offset") << writeResult.offset <<
        key("length") << writeResult.len <<
        key("write_operation_type") << toString(writeResult.writeOp) <<
        key("data");
    return serializeRawDataToHexString(os, writeResult.data, writeResult.len) << endObject;
}
