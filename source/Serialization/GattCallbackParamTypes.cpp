#include <stddef.h>
#include "GattCallbackParamTypes.h"
#include "Serialization/Hex.h"

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

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GattWriteCallbackParams& writeResult) {
    using namespace serialization;

    os << startObject <<
        key("connection_handle") << writeResult.connHandle <<
        key("attribute_handle") << writeResult.handle <<
        key("offset") << writeResult.offset <<
        key("length") << writeResult.len <<
        key("write_operation_type") << writeResult.writeOp <<
        key("data");
    return serializeRawDataToHexString(os, writeResult.data, writeResult.len) << endObject;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, GattWriteCallbackParams::WriteOp_t writeOperation) {
    switch(writeOperation) {
        case GattWriteCallbackParams::OP_INVALID:
            return os << "OP_INVALID";
        case GattWriteCallbackParams::OP_WRITE_REQ:
            return os << "OP_WRITE_REQ";
        case GattWriteCallbackParams::OP_WRITE_CMD:
            return os << "OP_WRITE_CMD";
        case GattWriteCallbackParams::OP_SIGN_WRITE_CMD:
            return os << "OP_SIGN_WRITE_CMD";
        case GattWriteCallbackParams::OP_PREP_WRITE_REQ:
            return os << "OP_PREP_WRITE_REQ";
        case GattWriteCallbackParams::OP_EXEC_WRITE_REQ_CANCEL:
            return os << "OP_EXEC_WRITE_REQ_CANCEL";
        case GattWriteCallbackParams::OP_EXEC_WRITE_REQ_NOW:
            return os << "OP_EXEC_WRITE_REQ_NOW";
        default:
            return os << "invalid GattWriteCallbackParams::WriteOp_t operation";
    }
}
