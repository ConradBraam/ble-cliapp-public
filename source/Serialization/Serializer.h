#ifndef BLE_CLIAPP_SERIALIZER_H_
#define BLE_CLIAPP_SERIALIZER_H_

#include <cstdlib>
#include <string.h>

#include "util/ConstArray.h"
#include "ble/Gap.h"



/**
 * @brief simple POD object which map a value to a string
 * @tparam T the type of value to map
 */
template<typename T>
struct ValueToStringMapping {
    T value;
    const char* str;
};

/**
 * @brief Base class for Serializer description, all Serializer description should specialize
 * this type.
 *
 * @tparam T The type on which this descripion apply
 */
template<typename T>
struct SerializerDescription;

/**
 * @brief simple serializer logic.
 *
 * @tparam T The type of the serialization. It require that SerializerDescription<T> exist
 */
template<typename T>
struct Serializer {
    typedef typename SerializerDescription<T>::type serialized_type;

    static const char* toString(const serialized_type val) {
        return toString(
            val,
            SerializerDescription<T>::mapping(),
            SerializerDescription<T>::errorMessage()
        );
    }

    static bool fromString(const char* str, serialized_type& val) {
        return fromString(
            str,
            val,
            SerializerDescription<T>::mapping()
        );
    }

private:
    static const char* toString(serialized_type value, const ConstArray<ValueToStringMapping<serialized_type> >& map, const char* error_message) {
        for(size_t i = 0; i < map.count(); ++i) {
            if(map[i].value == value) {
                return map[i].str;
            }
        }
        return error_message;
    }

    static bool fromString(const char* str, serialized_type& value, const ConstArray<ValueToStringMapping<serialized_type> >& map) {
        for(size_t i = 0; i < map.count(); ++i) {
            if(strcmp(map[i].str, str) == 0) {
                value = map[i].value;
                return true;
            }
        }
        return false;
    }
};

/**
 * @brief convert a value to a string
 */
template<typename T>
static const char* toString(const T& value) {
    return Serializer<T>::toString(value);
}

/**
 * @brief Convert a string to a value
 */
template<typename T>
static bool fromString(const char* str, T& value) {
    return Serializer<T>::fromString(str, value);
}

// Serialization functions from builtin types goes here
static bool fromString(const char* str, int8_t& val) {
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

// Serialization functions from builtin types goes here
static bool fromString(const char* str, uint16_t& val) {
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

// Serialization functions from builtin types goes here
static bool fromString(const char* str, uint32_t& val) {
    char* end;
    unsigned long tmp = strtoul(str, &end, 0);
    if(str == end) {
        return false;
    }

    val = (uint32_t) tmp;
    return true;
}

// Serialization functions from builtin types goes here
static bool fromString(const char* str, bool& val) {
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

#endif //BLE_CLIAPP_SERIALIZER_H_
