#include <cstdlib>
#include <string.h>

#include "Serializer.h"

bool fromString(const char* str, int8_t& val) {
    char* end;
    long tmp = strtol(str, &end, 0);
    if(str == end) {
        return false;
    }

    if(tmp < INT8_MIN || tmp > INT8_MAX) {
        return false;
    }

    val = (int8_t) tmp;
    return true;
}

bool fromString(const char* str, uint16_t& val) {
    char* end;
    unsigned long tmp = strtoul(str, &end, 0);
    if(str == end) {
        return false;
    }

    if(tmp > UINT16_MAX) {
        return false;
    }

    val = (uint16_t) tmp;
    return true;
}

bool fromString(const char* str, uint32_t& val) {
    char* end;
    unsigned long tmp = strtoul(str, &end, 0);
    if(str == end) {
        return false;
    }

    val = (uint32_t) tmp;
    return true;
}

bool fromString(const char* str, bool& val) {
    if(strcmp(str, "true") == 0) {
        val = true;
        return true;
    }

    if(strcmp(str, "false") == 0) {
        val = false;
        return true;
    }

    return false;
}

