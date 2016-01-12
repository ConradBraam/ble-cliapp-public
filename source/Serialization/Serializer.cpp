#include <cstdlib>
#include <cstring>
#include <limits>

#include "Serializer.h"

using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::strtoul;
using std::strtol;
using std::strcmp;

template<typename T>
using numeric_limits = std::numeric_limits<T>;

bool fromString(const char* str, int8_t& val) {
    char* end;
    long tmp = strtol(str, &end, 0);
    if(str == end) {
        return false;
    }

    if(tmp < numeric_limits<int8_t>::min() || tmp > numeric_limits<int8_t>::max()) {
        return false;
    }

    val = (int8_t) tmp;
    return true;
}

bool fromString(const char* str, uint8_t& val) {
    char* end;
    long tmp = strtol(str, &end, 0);
    if(str == end) {
        return false;
    }

    if(tmp < numeric_limits<uint8_t>::min() || tmp > numeric_limits<uint8_t>::max()) {
        return false;
    }

    val = (uint8_t) tmp;
    return true;
}


bool fromString(const char* str, uint16_t& val) {
    char* end;
    unsigned long tmp = strtoul(str, &end, 0);
    if(str == end) {
        return false;
    }

    if(tmp > numeric_limits<uint16_t>::max()) {
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

