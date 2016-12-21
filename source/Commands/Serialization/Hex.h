#ifndef BLE_CLIAPP_SERIALIZATION_HEX_H_
#define BLE_CLIAPP_SERIALIZATION_HEX_H_

#include "util/Vector.h"
#include "JSONOutputStream.h"

/**
 * @brief Convert the string representation of a byte in asci hexadecimal
 * characters to a byte.
 *
 * @param msb The ascii character representing the msb part of the byte
 * @param lsb The ascii character representing the lsb part of the byte
 * @param result the variable which will be filled with the result if the
 * conversion succeed.
 * @return true if the conversion has been made and false otherwise.
 */
bool asciiHexByteToByte(char msb, char lsb, uint8_t& result);

/**
 * @brief convert an arary of bytes to its representation as an hexadecimal string

 * @param data The data to convert
 * @param length The length of the data to convert
 *
 * @return The data as an hexadecimal string
 */
serialization::JSONOutputStream& serializeRawDataToHexString(serialization::JSONOutputStream& os, const uint8_t* data, std::size_t length);

/**
 * @brief Convert the string representation of bytes in ascii hexadecimal to
 * an array of bytes.
 *
 * @param data data to convert, it should be null terminated
 *
 * @return The converted data, if input data were invalid, the returned Vector is
 * invalid.
 */
container::Vector<uint8_t> hexStringToRawData(const char* data);

typedef container::Vector<uint8_t> RawData_t;

static inline bool fromString(const char* str, RawData_t& value) { 
    container::Vector<uint8_t> tmp = hexStringToRawData(str);
    if (tmp.size() == 0) { 
        return false;
    }
    value = tmp;
    return true;
}


#endif //BLE_CLIAPP_SERIALIZATION_HEX_H_
