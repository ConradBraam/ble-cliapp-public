#include <cstdlib>
#include <cstdint>
#include <cstdio>

#include "Hex.h"

bool asciiHexByteToByte(char msb, char lsb, uint8_t& result) {
    char hexStr[] = { msb, lsb, 0 };
    char* end;

    long conversionResult = strtol(hexStr, &end, 16);
    if (end == hexStr) {
        return false;
    }

    result = (uint8_t) conversionResult;
    return true;
}

container::SimpleString rawDataToHexString(const uint8_t* data, size_t length) {
    container::SimpleString result;

    if(!length) {
        return result;
    }

    char* hexData = (char*) malloc((length * 2) + 1);
    for (size_t i = 0; i < length; ++i) {
        snprintf(hexData + (i * 2), 3, "%02X", data[i]);
    }

    container::SimpleString result = hexData;
    free(hexData);
    return result;
}

container::Vector<uint8_t> hexStringToRawData(const char* data) {
    container::Vector<uint8_t> result;

    size_t strLen = strlen(data);

    if (strLen % 2) {
        return result;
    }

    for(size_t i = 0; i < strLen; i += 2) {
        uint8_t convertedByte;
        if(asciiHexByteToByte(data[i], data[i + 1], convertedByte) == false) {
            // this is for RVO
            result = container::Vector<uint8_t>();
            return result;
        }
        result.push_back(convertedByte);
    }

    return result;
}
