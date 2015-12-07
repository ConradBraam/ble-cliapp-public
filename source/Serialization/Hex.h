#ifndef BLE_CLIAPP_SERIALIZATION_HEX_H_
#define BLE_CLIAPP_SERIALIZATION_HEX_H_

#include "util/SimpleString.h"

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
container::SimpleString rawDataToHexString(const uint8_t* data, size_t length);


#endif //BLE_CLIAPP_SERIALIZATION_HEX_H_
