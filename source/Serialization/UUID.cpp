#include <algorithm>
#include <cstdio>

#include "UUID.h"
#include "Hex.h"
#include "Serializer.h"

using container::SimpleString;

namespace {
static bool longUUIDfromString(const char* str, UUID& uuid) {
    UUID::LongUUIDBytes_t uuidData;

    for(size_t i = 0, currentStrIndex = 0; i < sizeof(uuidData); ++i, currentStrIndex += 2) {
        // check for separator
        if(currentStrIndex == 8 || currentStrIndex == 13 || currentStrIndex == 18 || currentStrIndex == 23) {
            if(str[currentStrIndex] != '-') {
                return false;
            }
            ++currentStrIndex;
        }

        if(!asciiHexByteToByte(str[currentStrIndex], str[currentStrIndex + 1], uuidData[i])) {
            return false;
        }
    }

    std::reverse(uuidData, uuidData + sizeof(uuidData));
    uuid = UUID(uuidData);

    return true;
}

static SimpleString toStringLongUUID(const uint8_t* data) {
    char uuidStr[sizeof("XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX")];

    snprintf(uuidStr, sizeof(uuidStr),
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        data[15], data[14], data[13], data[12], data[11], data[10], data[9], data[8],
        data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0]);

    return uuidStr;
}

static bool shortUUIDFromString(const char* str, UUID& uuid) {
    UUID::ShortUUIDBytes_t uuidValue;

    if(!fromString(str, uuidValue)) {
        return false;
    }

    uuid = UUID(uuidValue);
    return true;
}

static SimpleString toStringShortUUID(uint16_t uuid) {
    char uuidStr[2 + sizeof(uint16_t) * 2 + 1] = { 0 };
    snprintf(uuidStr, sizeof(uuidStr), "0x%04X", uuid);
    return uuidStr;
}
}

bool fromString(const char* str, UUID& uuid) {
    if(strlen(str) == (sizeof("XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX") - 1)) {
        return longUUIDfromString(str, uuid);
    } else {
        return shortUUIDFromString(str, uuid);
    }
}

SimpleString toString(const UUID& uuid) {
    if(uuid.shortOrLong() == UUID::UUID_TYPE_SHORT) {
        return toStringShortUUID(uuid.getShortUUID());
    } else {
        return toStringLongUUID(uuid.getBaseUUID());
    }
}

