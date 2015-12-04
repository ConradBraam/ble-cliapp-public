#include <cstdlib>
#include <cstdint>
#include "Hex.h"

bool asciiHexByteToByte(char msb, char lsb, uint8_t& result) {
    char hexStr[] = { msb, lsb, 0 };
    char* end;

    long conversionResult = strtol(hexStr, &end, 16);
    if(end == hexStr) {
        return false;
    }

    result = (uint8_t) conversionResult;
    return true;
}
