#ifndef BLE_CLIAPP_SERIALIZER_H_
#define BLE_CLIAPP_SERIALIZER_H_

#include "util/ConstArray.h"
#include <cstring>

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
        for(std::size_t i = 0; i < map.count(); ++i) {
            if(map[i].value == value) {
                return map[i].str;
            }
        }
        return error_message;
    }

    static bool fromString(const char* str, serialized_type& value, const ConstArray<ValueToStringMapping<serialized_type> >& map) {
        for(std::size_t i = 0; i < map.count(); ++i) {
            if(std::strcmp(map[i].str, str) == 0) {
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
static inline bool fromString(const char* str, T& value) {
    return Serializer<T>::fromString(str, value);
}

/**
 * @brief Convert a string to an in8_t
 *
 * @param[in] str The string to convert
 * @param[out] val The result of the conversion
 *
 * @return true if the conversion succeed and false otherwise
 */
bool fromString(const char* str, std::int8_t& val);

/**
 * @brief Convert a string to an uin8_t
 *
 * @param[in] str The string to convert
 * @param[out] val The result of the conversion
 *
 * @return true if the conversion succeed and false otherwise
 */
bool fromString(const char* str, std::uint8_t& val);

/**
 * @brief Convert a string to an uint16_t
 *
 * @param[in] str The string to convert
 * @param[out] val The result of the conversion
 *
 * @return true if the conversion succeed and false otherwise
 */
bool fromString(const char* str, std::uint16_t& val);

/**
 * @brief Convert a string to an uint32_t
 *
 * @param[in] str The string to convert
 * @param[out] val The result of the conversion
 *
 * @return true if the conversion succeed and false otherwise
 */
bool fromString(const char* str, std::uint32_t& val);

/**
 * @brief Convert a string to a bool
 *
 * @param[in] str The string to convert
 * @param[out] val The result of the conversion
 *
 * @return true if the conversion succeed and false otherwise
 */
bool fromString(const char* str, bool& val);

#endif //BLE_CLIAPP_SERIALIZER_H_
